/*
COMSC-210 | Lab 19 | Rylan Der
IDE Used: Visual Studio Code
 */

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <istream>
#include <sstream>

 using namespace std;

 const int STREAM_IGNORE_CHARS = 100;

 struct ReviewNode {
    string comment;
    double rating;
    ReviewNode* next;

    //constructors; by default next is initialized with nullptr
    ReviewNode() { comment = "No comment"; rating = -1; next = nullptr; }
    ReviewNode(string c, double r): comment(c), rating(r), next(nullptr) { }
    ReviewNode(string c, double r, ReviewNode* next): comment(c), rating(r), next(next) { }
 };

void fillReviews(istream* input, ReviewNode* &head, int size);

void push_front(ReviewNode* &head, double rating, string comment);

void push_back(ReviewNode* &head, double rating, string comment);

void outputReviews(ReviewNode* head);

template <typename T>
T validateRange(istream* input, string datatype, T min, T max);

int main() {
    const string FILENAME = "";
    istream* input;
    ifstream infile;

    bool pushFront = false;     //Flag to determine whether to append or prepend list elements
    string continueFlag = "";   //Whether or not user wants to quit

    ReviewNode* head = nullptr; 

    double rating;              //temp variables when taking input
    string comment;

    //Choose between file or console input
    if(FILENAME != "") {
        infile.open(FILENAME);
        //validate file open
        if (!infile.is_open()) {
            cout << "ERROR: Could not open file " << FILENAME << endl;
            throw ios_base::failure("Invalid file name");
        }
        input = &infile;
    } else {
        input = &cin;
    }

    //Determine prepend or append
    cout << "Which linked list method should we use?" << endl
         << "\t[1] New nodes are added at the head of the linked list" << endl
         << "\t[2] New nodes are added at the tail of the linked list" << endl
         << "\tChoice:" << endl;
    pushFront = validateRange(input, "integer", 1, 2) == 1;

    //Take reviews from input while user continues
    do {
        cout << "Enter review rating 0-5: " << endl;
        rating = validateRange(input, "double", 0.0, 5.0);

        cout << "Enter review comments: " << endl;
        getline(*input, comment);

        cout << "Enter another review? Y/N: " << endl;
        getline(*input, continueFlag);
        
        if (pushFront) {
            push_front(head, rating, comment);
        } else {
            push_back(head, rating, comment);
        }
    } while (continueFlag == "y" || continueFlag == "Y");

    outputReviews(head);
}

/**
 * Prepends a new review node to the front of a linked list
 * @param head Head of the linked list to prepend to
 * @param rating Rating of the new node
 * @param comment Comment of the new node
 */
void push_front(ReviewNode* &head, double rating, string comment) {
    ReviewNode* newNode = new ReviewNode(comment, rating); //initialized w/ nullptr by default
    if (head) {
        newNode->next = head; //if list is not empty, only need to update next pointer
    }
    head = newNode; //otherwise works if list is empty
}

/**
 * Appends a new review node to the end of a linked list
 * @param head Head of the linked list to append to
 * @param rating Rating of the new node
 * @param comment Comment of the new node
 */
void push_back(ReviewNode* &head, double rating, string comment) {
    ReviewNode* current = head;
    ReviewNode* newNode = new ReviewNode(comment, rating); //initialized w/ nullptr by default
    if (!head) { //head is the new node if empty
        head = newNode;
    } else { //otherwise traverse & add to end
        while (current->next) { current = current->next; }
        current->next = newNode;
    }
}

/**
 * Outputs reveiws in a linked list followed by the average review
 * @param head Head of the linked list of reviews
 */
void outputReviews(ReviewNode* head) {
    ReviewNode *current = head;
    int reviewNumber = 0; //keep track of # reviews while traversing
    double sum = 0;       //sum reviews to calculate average

    cout << fixed << setprecision(2);
    cout << "Outputting all reviews:" << endl;

     //traverse list
    while (current) {
        reviewNumber++;
        cout << "\t> Review #" << reviewNumber << ": " << current->rating << ": " << current->comment << endl;
        sum += current->rating;
        current = current->next;
    }
    cout << "\t> Average: " << sum / reviewNumber << endl;
}

/**
 * Validates input from range min to max inclusive
 * @param input Data is taken from input and validated
 * @param datatype Name of datatype when displaying error message
 * @param min Minimum value to accept
 * @param max Maximum value to accept
 * @return Validated piece of data
 */
template <typename T>
T validateRange(istream* input, string datatype, T min, T max) {
    T val = min;                //Initialize with min to prevent error message on first loop iteration
    string inputString = "";
    stringstream ss;            //Uses stringstream to prevent fall through of values remaining in the given input stream
    do {
        //Check for valid messages at beginning to clear ss of errors right before data input
        //Allows while loop to check for a failed stream without another flag
        if (ss.fail()) {
            cout << "\tInput must be type " << datatype << endl;
            ss.clear();
        } else if (val < min || val > max) { //range of min - max is inclusive
            cout << "\tInput must be in range " << min << " - " << max << endl;
            ss.clear();
        }
        ss.str(""); //reset stringstream

        cout << "\t> ";
        getline(*input, inputString);
        ss.str(inputString);
        ss >> val;
    } while (ss.fail() || val < min || val > max);
    return val;
}

/**
 * Fills a linked list of reviews, taking comments from an input stream, 
 * and populating the rating with a random double from 0-5 (inclusive).
 * @param input Input stream to take comments from
 * @param head Head node of the linked list to populate
 * @param size Number of reviews to add
 */
void fillReviews(istream* input, ReviewNode* &head, int size) {
    static const int MIN_RATING = 0;
    static const int MAX_RATING = 5;

    string comment;
    double rating;
    for(int i = 0; i < size; i++) {
        MIN_RATING + (rand() % (10 * MAX_RATING + 1)) / 10.0;
        getline(*input, comment);
        push_front(head, rating, comment);
    } 
}