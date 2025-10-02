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
 
 struct ReviewNode;
 class Movie;
 struct MovieNode;

 class Movie {
    public:
        //Constructors
        Movie() { name = ""; head = nullptr; }
        Movie(string name): name(name), head(nullptr) { }
        Movie(string name, istream* input, int reviews);

        //Deltes nodes in linked list of reviews
        ~Movie();

        //Getters
        string GetName() const { return name; }
        ReviewNode* GetReviews() const { return head; }

        //Displays movie contents
        void Print() const;

    private:
        //Helper functions for output and object instantiation
        void outputReviews() const;
        void fillReviews(istream* input, int size);
        void push_front(double rating, string comment);
        void push_back(double rating, string comment);
        void deleteReviews();

        string name;
        ReviewNode* head;
 };

 struct ReviewNode {
    string comment;
    double rating;
    ReviewNode* next;

    //constructors; by default next is initialized with nullptr
    ReviewNode() { comment = "No comment"; rating = -1; next = nullptr; }
    ReviewNode(string c, double r): comment(c), rating(r), next(nullptr) { }
    ReviewNode(string c, double r, ReviewNode* next): comment(c), rating(r), next(next) { }
 };

struct MovieNode {
    MovieNode* next;
    Movie* movie;

    MovieNode() { next = nullptr; }
    //Instantiates with a full movie object
    MovieNode(string name, istream* input, int size) { movie = new Movie(name, input, size); next = nullptr; }
    ~MovieNode() { delete movie; }
};

void appendMovieNode(MovieNode* &head, MovieNode* &newNode);

void deleteMovieList(MovieNode* &head);

void fillMovieList(istream* nameInput, istream* reviewInput, MovieNode* &head, int size, int numReviews);

void outputMovieList(MovieNode* head);

template <typename T>
T validateRange(istream* input, string datatype, T min, T max);

int main() {
    srand(time(0));
    const int REVIEWS = 5;
    const int MOVIES = 5;
    const string BAD_REVIEWS_FILENAME = "badreviews.txt";
    const string GOOD_REVIEWS_FILENAME = "goodreviews.txt";
    const string NAMES_FILENAME = "names.txt";
    ifstream nameFile;
    ifstream badReviewFile;
    ifstream goodReviewFile;

    MovieNode* head = nullptr;
    
    //validate file open for reviews and names (if used)
    goodReviewFile.open(GOOD_REVIEWS_FILENAME);
    if (!goodReviewFile.is_open()) {
        cout << "ERROR: Could not open file " << GOOD_REVIEWS_FILENAME << endl;
        throw ios_base::failure("Invalid file name");
    }
    badReviewFile.open(BAD_REVIEWS_FILENAME);
    if (!badReviewFile.is_open()) {
        cout << "ERROR: Could not open file " << BAD_REVIEWS_FILENAME << endl;
        throw ios_base::failure("Invalid file name");
    }
    nameFile.open(NAMES_FILENAME);
    if (!nameFile.is_open()) {
        cout << "ERROR: Could not open file " << NAMES_FILENAME << endl;
        throw ios_base::failure("Invalid file name");
    }

    fillMovieList(&nameFile, &badReviewFile, head, MOVIES, REVIEWS);
    outputMovieList(head);
}

/**
 * Prepends a new review node to the front of the reviews linked list
 * @param rating Rating of the new node
 * @param comment Comment of the new node
 */
void Movie::push_front(double rating, string comment) {
    ReviewNode* newNode = new ReviewNode(comment, rating); //initialized w/ nullptr by default
    if (head) {
        newNode->next = head; //if list is not empty, only need to update next pointer
    }
    head = newNode; //otherwise works if list is empty
}

/**
 * Appends a new review node to the end of a linked list
 * @param rating Rating of the new node
 * @param comment Comment of the new node
 */
void Movie::push_back(double rating, string comment) {
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
 */
void Movie::outputReviews() const{
    ReviewNode* current = head;
    int reviewNumber = 0; //keep track of # reviews while traversing
    double sum = 0;       //sum reviews to calculate average

    cout << fixed << setprecision(2);
    cout << " > Reviews:" << endl;

     //traverse list
    while (current) {
        reviewNumber++;
        cout << "\t#" << reviewNumber << ": " << current->rating << ": " << current->comment << endl;
        sum += current->rating;
        current = current->next;
    }
    cout << " > Average: " << sum / reviewNumber << endl;
}

/**
 * Outputs linked list of movies
 * @param head Head node of the linked list to output
 */
void outputMovieList(MovieNode* head) {
    MovieNode* current = head;
    int movieNumber = 0;
    //treverse list; use movie print function for output
    while(current) {
        movieNumber++;
        cout << "Movie #" << movieNumber << ": " << endl;
        current->movie->Print();
        current = current->next;
    }
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
void Movie::fillReviews(istream* input, int size) {
    static const int MIN_RATING = 0;
    static const int MAX_RATING = 5;

    string comment;
    double rating;
    for(int i = 0; i < size; i++) {
        rating = MIN_RATING + (rand() % (10 * MAX_RATING + 1)) / 10.0;
        getline(*input, comment);
        push_front(rating, comment);
    } 
}

/**
 * Fills a linked list of movies from input
 * @param nameInput Movie names are retreived from this input stream
 * @param reviewInput Reviews are retreived from this input stream
 * @param head Head node of the linked list of movies
 * @param size Length of the linked list of movies
 * @param numReviews Number of reviews for each movie
 */
void fillMovieList(istream* nameInput, istream* reviewInput, MovieNode* &head, int size, int numReviews) {
    string name;
    MovieNode* newNode;
    for (int i = 0; i < size; i++) {
        cout << "Enter movie name: " << endl;
        getline(*nameInput, name);                              //get name from input
        newNode = new MovieNode(name, reviewInput, numReviews); //Calls movie constructor when inputting reviews
        appendMovieNode(head, newNode);
    }
}

/**
 * Instantiates a movie object and populates it with reviews from input
 * @param name Name of the movie
 * @param input Retrieves review comments from input stream
 * @param reviews Number of reviews to add
*/
Movie::Movie(string name, istream* input, int reviews) {
    this->name = name;
    head = nullptr;
    fillReviews(input, reviews);
}

/**
 * Outputs to console movie title and reviews
 */
void Movie::Print() const{
    cout << "Title: " << name << endl;
    outputReviews();
}

/**
 * Deletes all nodes in the linked list of reviews
 */
Movie::~Movie() {
    deleteReviews();
}

/**
 * Delete linked list of reviews stored
 */
void Movie::deleteReviews() {
    ReviewNode* current;
    //traverse & delete pointers
    while (head) {
        current = head;
        head = head->next;
        delete current;
    }
    head = nullptr;
}

/**
 * Appends a given MovieNode to a linked list of movies
 * @param head Head node of the linked list
 * @param newNode Instantiated MovieNode to append
 */
void appendMovieNode(MovieNode* &head, MovieNode* &newNode) {
    if (head) {
        newNode->next = head;
    }
    head = newNode;
}

/**
 * Deletes nodes of given linked list of movies
 */
void deleteMovieList(MovieNode* &head) {
    MovieNode* current;
    //treaverse & delete pointers
    while(head) {
        current = head;
        head = head->next;
        delete current;
    }
    head = nullptr;
}