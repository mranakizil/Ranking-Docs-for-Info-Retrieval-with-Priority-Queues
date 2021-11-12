// CSE2025 - Project #2
// Merve Rana Kýzýl
// 150119825

#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include<malloc.h>

typedef struct Node {
    int number_of_words;
    char* filename;
    int degree;
    struct Node* parent;
    struct Node* child;
    struct Node* sibling;

} Node;

int readWords(char*, char*, int);
Node* newNode(int, char*);
Node* newBinomialHeap();
Node* insertIntoBinomialHeap(Node*, Node*);
void binomialLink(Node*, Node*);
Node* binomialHeapUnion(Node*, Node*);
Node* mergeTwoBinomialHeaps(Node*, Node*);
Node* extractMax(Node*);
Node* reverseList(Node*);

int main() {
	
	// Create a new binomial heap
    Node* BH = newBinomialHeap();
    int total_no_of_relevant_docs = 0;

    char keyword[1000];
    printf("Enter the keyword: ");
    scanf("%s", keyword);
    
    int no_of_relevant_docs;
    printf("Enter the number of relevant documents: ");
    scanf("%d", &no_of_relevant_docs);

	// Pointer for directory entry
    struct dirent *de; 
    DIR *dr = opendir("files");

    if (dr == NULL) {
        printf("Could not open current directory");
        return 0;
    }

    while ((de = readdir(dr)) != NULL) {
    
        char filename[1000];

        strcpy(filename, de->d_name);

        if (strcmp(filename, ".") != 0 && strcmp(filename, "..") != 0) {
			
			// Find the number of keywords in a file
            int number_of_words = readWords(filename, keyword, 0);
            // Create a new node
            Node* node = newNode(number_of_words, filename);
            // Insert the node into the binomial heap
            BH = insertIntoBinomialHeap(BH, node);
            
            if(number_of_words != 0)
            	total_no_of_relevant_docs++;
    
        }
    }
    
    printf("\nNUMBER OF RELEVANT DOCUMENTS: %d\n", total_no_of_relevant_docs); 
    
    int i;
    for(i = 0; i < no_of_relevant_docs; i++){
    	
    	// Extract the relevant documents
	    BH = extractMax(BH);
	}
    
    closedir(dr);
    return 0;
}

// Reads words from the given file if print is 0
// Prints the contents of the given file if print is 1
int readWords(char *filename, char *keyword, int print) {
    int word_counter = 0;
    char str[1000] = {0};
    strcat(str, "files\\");
    strcat(str, filename);
  
    FILE *fptr;
    if ((fptr = fopen(str, "r")) == NULL) {
        printf("Could not open the file!");
        return -1;
    }
    
    if(print) {
	    // Read contents from file 
	    char c = fgetc(fptr); 
	    while (c != EOF) { 
	        printf ("%c", c); 
	        c = fgetc(fptr); 
	    }   	
	}
	else{
		char word[1000];
	    while (fscanf(fptr, " %100[a-zA-Z]%*[^a-zA-Z]", word) == 1) {
	        if(strcmp(word, keyword) == 0) {
	            word_counter++;
	        }
    	}	
	}
	
    fclose(fptr);
    return word_counter;
}

// Creates a new node
Node* newNode(int number_of_words, char* filename) {
    Node* node = (Node*) malloc(sizeof(Node));
    node->number_of_words = number_of_words;
    node->filename = strdup(filename);
    return node;
}

// Creates a new binomial heap
Node* newBinomialHeap() {
    Node* BH = NULL;
    return BH;
}

// Inserts the given node into the binomial heap
Node* insertIntoBinomialHeap(Node* BH, Node* node) {
    Node* BH1 = newBinomialHeap();
    node->parent = NULL;
    node->child = NULL;
    node->sibling = NULL;
    node->degree = 0;
    BH1 = node;
    // Free the temporary binary heap BH1
    BH = binomialHeapUnion(BH, BH1);
    return BH;
}

// Links binomial trees whose roots have the same degree
// Makes binomial_tree2 the parent of binomial_tree1
void binomialLink(Node* binomial_tree1, Node* binomial_tree2) {
    binomial_tree1->parent = binomial_tree2;
    binomial_tree1->sibling = binomial_tree2->child;
    binomial_tree2->child = binomial_tree1;
    binomial_tree2->degree = binomial_tree2->degree + 1;
}

// Unites two binomial heaps and returns the united binomial heap
Node* binomialHeapUnion(Node* BH1, Node* BH2) {
    Node* prev_node;
    Node* next_node;
    Node* node;
    Node* united_BH = newBinomialHeap();
    // Merge the root lists of the binomial heaps
    united_BH = mergeTwoBinomialHeaps(BH1, BH2);
    if (united_BH == NULL)
        return united_BH;
    prev_node = NULL;
    node = united_BH;
    next_node = node->sibling;
    while (next_node != NULL) {
        if ((node->degree != next_node->degree) || ((next_node->sibling != NULL) && (next_node->sibling)->degree == node->degree)) {
            prev_node = node;
            node = next_node;
        } else {
            if (node->number_of_words >= next_node->number_of_words) {
                node->sibling = next_node->sibling;
                binomialLink(next_node, node);
            } else {
                if (prev_node == NULL)
                    united_BH = next_node;
                else
                    prev_node->sibling = next_node;
                binomialLink(node, next_node);
                node = next_node;
            }
        }
        next_node = node->sibling;
    }
    return united_BH;
}

// Merges the given binomial heaps
Node* mergeTwoBinomialHeaps(Node* BH1, Node* BH2) {
    Node* BH = NULL;
    Node** heap = &BH;
    while (BH1 != NULL && BH2 != NULL) {
        if (BH1->degree < BH2->degree) {
            *heap = BH1;
            BH1 = BH1->sibling;
        }
        else {
            *heap = BH2;
            BH2 = BH2->sibling;
        }
        heap = &(*heap)->sibling;
    }
    if (BH1 != NULL)
        *heap = BH1;
    else
        *heap = BH2;
    
    return BH;
}


// Extracts the node with maximum key
Node* extractMax(Node* BH){
	
	if (BH == NULL)
       return NULL;
  
    Node *prevMaxRoot = NULL; 
    Node *maxRoot = BH; 
  
    // Find the node with maximum key
    int max = maxRoot->number_of_words; 
    Node *current = BH; 
    while (current->sibling != NULL) { 
        if ((current->sibling)->number_of_words > max) { 
            max = (current->sibling)->number_of_words; 
           	prevMaxRoot = current; 
            maxRoot = current->sibling; 
        } 
        current = current->sibling; 
    }
    
    if(maxRoot->number_of_words > 0){
    	
    	printf("\n%s(%d): ", maxRoot->filename, maxRoot->number_of_words);
	    readWords(maxRoot->filename, "", 1);
	    printf("---------------------------------------------------\n\n");
	}
    
    // If there is only one node in the binomial heap
    if (prevMaxRoot == NULL && maxRoot->sibling == NULL) 
        BH = NULL; 
    else if (prevMaxRoot == NULL) 
        BH = maxRoot->sibling;  
    else
    	// Remove the node with maximum key from root list 
        prevMaxRoot->sibling = maxRoot->sibling;
		
	Node* BH1 = newBinomialHeap();
	// Reverse linked list of maxRoot's children
	BH1 = reverseList(maxRoot->child);
	BH = binomialHeapUnion(BH, BH1);
	return BH;
}

// Reverses the order of the linked list of the given node
Node* reverseList(Node *child) { 
 	Node* sibling;
   	Node* last_node = NULL;
    
    if (child == NULL)
        return child;
    
    child->parent = NULL;
    while (child->sibling != NULL) {
        sibling = child->sibling;
        child->sibling = last_node;
        child->sibling = last_node;
        last_node = child;
        child = sibling;
        child->parent  = NULL;
    }
    child->sibling = last_node;
    return child;
}


