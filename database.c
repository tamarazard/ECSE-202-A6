/**database.c
 *
 *  Created on: Nov. 26, 2019
 *      Author: tamara
 *      Description : A simple program to manage a small database of student
 *            	 	: records using B-Trees for storage. A part of this code was copied from the
 *            	 	: dbReader.c file provided for the assignment
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// To make the "C" implementation completely analogous to Java, one has to create
// an object for each student record and attach it to a corresponding bNode
// object in a B-Tree data structure.  These objects are represented by the
// corresponding structure templates below.

#define MAXLEN 20
#define false 0
#define true !false



// Prototypes and templates should go in a file called sortFile.h which
// is subsequently included in sortFile.c.  For a small program like this one,
// a single file will do.

// Structure Templates

typedef struct SR {				// The student record object
    char Last[MAXLEN];
	char First[MAXLEN];
	int ID;
	int marks;
} SRecord;

typedef struct bN {				// The bNode object (not used
	struct SR *Srec;			// in this demo, but you will
	struct bN *left;			// need it for A6).
	struct bN *right;
} bNode;

// Function Prototypes
//
// Notice that there are two versions of add_node, inorder, and search.
// This is to accommodate sorting the database by last name OR student ID
// You will have to write these for a6.

bNode *addNode_Name(bNode *root, SRecord *Record);
bNode *addNode_ID(bNode *root, SRecord *Record);
bNode *makeNode(SRecord *data);

void inorder(bNode *root);
void search_Name(bNode *root, char *data);
void search_ID(bNode *root, int ID);
void str2upper(char *string);
void help();

//
// Since we haven't done pointer-pointers in this course, we'll use a
// global variable to return the matching student record.  This is
// equivalent to what we did in Java using an instance variable.

bNode *match;


// Main entry point is here.  Program uses the standard Command Line Interface

int main(int argc, char *argv[]) {

// Internal declarations

    FILE * NAMESIDS;        // File descriptor (an object)!
	FILE * MARKS;           // Will have two files open

    bNode *root_N;   		// Pointer to names B-Tree
    bNode *root_I;   		// Pointer to IDs B-Tree
    SRecord *Record;   		// Pointer to current record read in

	int NumRecords;
	char cmd[MAXLEN], sName[MAXLEN];
	int sID;

// Argument check
        if (argc != 3) {
                printf("Usage: sdb [Names+IDs] [marks] \n");
                return -1;
        }

// Attempt to open the user-specified file.  If no file with
// the supplied name is found, exit the program with an error
// message.

        if ((NAMESIDS=fopen(argv[1],"r"))==NULL) {
                printf("Can't read from file %s\n",argv[1]);
                return -2;
        }

        if ((MARKS=fopen(argv[2],"r"))==NULL) {
                printf("Can't read from file %s\n",argv[2]);
                fclose(NAMESIDS);
                return -2;
        }

// Initialize B-Trees by creating the root pointers;

    root_N=NULL;
	root_I=NULL;

//  Read through the NamesIDs and marks files, record by record.

	NumRecords=0;

	printf("Building database...\n");

	while(true) {

// 	Allocate an object to hold the current data

		Record = (SRecord *)malloc(sizeof(SRecord));
		if (Record == NULL) {
			printf("Failed to allocate object for data in main\n");
			return -1;
		}

//  Read in the data.  If the files are not the same length, the shortest one
//  terminates reading.

		int status = fscanf(NAMESIDS,"%s%s%d",Record->First,Record->Last,&Record->ID);
		if (status == EOF) break;
		status = fscanf(MARKS,"%d",&Record->marks);
		if (status == EOF) break;
		NumRecords++;

//	Add the record just read in to 2 B-Trees - one ordered
//  by name and the other ordered by student ID.

    root_N=addNode_Name(root_N,Record);
    root_I=addNode_ID(root_I,Record);

//	For this demo we'll simply list each record as we receive it

		printf("\nStudent Name:\t\t%s %s\n",Record->First,Record->Last);
		printf("Student ID:\t\t%d\n",Record->ID);
		printf("Total Grade:\t\t%d\n",Record->marks);

	}

// Close files once we're done

	fclose(NAMESIDS);
	fclose(MARKS);

	printf("Finished, %d records found...\n",NumRecords);


//
//  Simple Command Interpreter: - This is commented out until you implement the functions listed above
//

	while (1) {
	    printf("sdb> ");
	    scanf("%s",cmd);					  // read command
	    str2upper(cmd);

	    if (0) {							  // This is a temporary stub
	    	continue;
	    }
                   // This directive hides the subsequent text from
                                // the compiler until it sees #endif
// List by Name

		if (strncmp(cmd,"LN",2)==0) {         // List all records sorted by name
			printf("Student Record Database sorted by Last Name\n\n");
			inorder(root_N);
			printf("\n");
		}

// List by ID

		else if (strncmp(cmd,"LI",2)==0) {    // List all records sorted by ID
			printf("Student Record Database sorted by Student ID\n\n");
			inorder(root_I);
			printf("\n");
		}

// Find record that matches Name

		else if (strncmp(cmd,"FN",2)==0) {    // List record that matches name
			printf("Enter name to search: ");
			scanf("%s",sName);
			match=NULL;
			search_Name(root_N,sName);
			if (match==NULL)
			  printf("There is no student with that name.\n\n");
	        else {
			  if (strlen(match->Srec->First)+strlen(match->Srec->Last)>15) {
				printf("\nStudent Name:\t%s %s\n",match->Srec->First,match->Srec->Last);
			  } else {
				printf("\nStudent Name:\t\t%s %s\n",match->Srec->First,match->Srec->Last);
			  }
			  printf("Student ID:\t\t%d\n",match->Srec->ID);
			  printf("Total Grade:\t\t%d\n\n",match->Srec->marks);
	        }
		}

// Find record that matches ID


		else if (strncmp(cmd,"FI",2)==0) {    // List record that matches ID
			printf("Enter ID to search: ");
			scanf("%d",&sID);
			match=NULL;
			search_ID(root_I,sID);
			if (match==NULL)
			  printf("There is no student with that ID.\n\n");
	        else {
			  if (strlen(match->Srec->First)+strlen(match->Srec->Last)>15) {
				printf("\nStudent Name:\t%s %s\n",match->Srec->First,match->Srec->Last);
			  } else {
				printf("\nStudent Name:\t\t%s %s\n",match->Srec->First,match->Srec->Last);
			  }
			printf("Student ID:\t\t%d\n",match->Srec->ID);
			printf("Total Grade:\t\t%d\n\n",match->Srec->marks);
	      }
		}



// Help

		else if (strncmp(cmd,"H",1)==0) {  // Help
			help();
		}

		else if (strncmp(cmd,"?",2)==0) {     // Help
			help();
		}

// Quit

		else if (strncmp(cmd,"Q",1)==0) {  // Help
			printf("Program terminated...\n");
			return 0;
		}

// Command not understood

		else {
			printf("Command not understood.\n\n");
		}
	}

}

//
//	Write and insert the functions listed in the prototypes here.
//
	bNode *makeNode (SRecord *data){							//method to create a new bNode
		bNode *node =(bNode *) malloc (sizeof(bNode));
		node->Srec = data;										//sets the data of the
		node->left = NULL;										//sets the right successor of the node to null
		node->right = NULL;										//sets the left successor of the node to null
		return node;
	}
	bNode *addNode_Name(bNode *root, SRecord *Record){			//method to add the names of the students in the b-tree
		int i;
		if (root==NULL){										//if the tree is empty
			root = makeNode(Record);							//create a new node and set it as the root
			return root;
		}
		else{													//if the tree is not empty
			bNode *current=root;								//add the node in the tree
			while (true){
				i = strcmp(Record->Last,current->Srec->Last);
				if (i<=0){										//if the data of the new node is less than the current one,
					if(current->left==NULL){					//if there is no left successor to the current node,
						current->left=makeNode(Record);			//create a node and set it as the left successor of the root
						return root;
					}
					else {
						current = current->left;				//else add it to the tree and set the node as the left successor
					}
				}
				else{											//if the data of the new node is greater than the current one,
					if(current->right == NULL){					//if there is no right successor to the current root,
						current->right = makeNode (Record);		//create a new node
						return root;
					}
					else{										//if there is a right successor,
						current = current->right;				//add the node to the tree and set it as a right successor
					}
				}
			}
		}
	}
	bNode *addNode_ID(bNode *root, SRecord *Record){			//this method adds the ID of the students in the b-tree
		if (root==NULL){										//if the tree is empty
			root = makeNode(Record);							//create a new node and set it as the root
			return root;
		}
		else{													//if the tree is not empty,
			bNode *current=root;								//set the current node as the root
			while (true){
				if (Record->ID <= current->Srec->ID){			//if the data of the new ID is less than or equal to the data of the current ID,
					if(current->left==NULL){					//if there is no left successor,
						current->left=makeNode(Record);			//create the node and set it as the left successor
						return root;
					}
					else {										//if there exist a left successor,
						current = current->left;				//set the new node as the left successor of the current node and set it
					}											//as the current node
				}
				else{											//if the data of the new ID is greater than the data of the current ID,
					if(current->right == NULL){					//if the right successor of the root is null
						current->right = makeNode (Record);		//create the new node and set it as the right successor of the current root
						return root;
					}
					else{										//if there exists a right successor
						current = current->right;				//set the new root to the right successot of the current node
					}
				}
			}
		}

	}
	void inorder(bNode *root){				//method to traverse the tree in order (LEFT-ROOT-RIGHT)
		if (root->left != NULL){			//if there exists left successors, traverse the tree to the left
			inorder(root->left);			//until the left successors is equal to null
		}
		//print the first name of the corresponding student, his last name, his ID number and his grades
		printf("%s %-20s\t %-10d\t %d\n",root->Srec->First, root->Srec->Last, root->Srec->ID, root->Srec->marks);

		if (root->right != NULL){			//if there exists right successors, traverse the tree to the righgt,
			inorder (root->right);			//until the right successors is equal to null
		}
	}

	void search_Name(bNode* root, char* data) {								//method to search the name of the student
		int i;
		if (root == NULL) {													//if the tree is empty, there is no students name
			//printf("\nThere is no student with this name");
			return;
		}
		else {																//if the tree is not empty,
			bNode* current = root;
			while (true) {
				i = strcmp(data, current->Srec->Last);						//compare the strings
				if (i == 0) {												//if the strings are equal
					match = current;										//the student/s name is the name stored in the current node
					break;
				}

				else if (i < 0) {											//if the data is less than the data of the current last name,
					if (current->left == NULL) {							//if the left successor is null, there is no student with that name
						//printf("\nThere is no student with this name");
						break;
					}
					else {													//if there is a left successor,
						current = current->left;							//set the current name to the left successor of the node
					}
				}
				else {														//if the data is greater than the data of the current last name
					if (current->right == NULL) {							//if there is no right successor, there is no student with that name
						//printf("\nThere is no student with this name");
						break;
					}
					else {													//if there is a right successor,
						current = current->right;							//set the current name to the right successor of the previous current name
					}
				}
			}

		}
	}
	void search_ID(bNode * root, int ID) {							//method to search a student with his ID
		if (root == NULL) {											//if the tree is empty, there is no students
			//printf("\nThere is no student with that ID");
			return;
		}
		else {														//if the tree is not empty,
			bNode* current = root;									//set the current node to root
			while (true) {
				if (ID == current->Srec->ID) {						//if the inserted ID corresponds to the ID stored in the current node,
					match = current;								//the student with this ID is found
					break;
				}

				else if (ID < current->Srec->ID) {							//if the inserted ID is less than the current ID
					if (current->left == NULL) {							//if there is no left successor, there is no student with this ID
						//printf("\nThere is no student with that ID");
						break;
					}
					else {													//if there exists a left successor,
						current = current->left;							//set it to the new current node
					}
				}
				else {														//if the inserted ID is greater than the ID stored in the current node,
					if (current->right == NULL) {							//if there is no right successor, then there is no student with this ID
						//printf("\nThere is no student with that ID");
						break;
					}
					else {													//if there exists a right successor,
						current = current->right;							//set it to the current node
					}
				}
			}

		}



	}



	//
	//  Convert a string to upper case
	//

	void str2upper(char* string) {
		int i;
		for (i = 0; i < strlen(string); i++)
			string[i] = toupper(string[i]);
		return;
	}

	// Help
	// prints command list

	void help() {
		printf("LN List all the records in the database ordered by last name.\n");
		printf("LI List all the records in the database ordered by student ID.\n");
		printf("FN Prompts for a name and lists the record of the student with the corresponding name.\n");
		printf("FI Prompts for a name and lists the record of the student with the Corresponding ID.\n");
		printf("HELP Prints this list.\n");
		printf("? Prints this list.\n");
		printf("Q Exits the program.\n\n");

		return;
	}

