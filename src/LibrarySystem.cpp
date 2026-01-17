
#include "LibrarySystem.h"
#include <iostream>
#include <algorithm>
#include <ctime>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cctype>

using namespace std;

// ==================== DATE IMPLEMENTATION ====================
Date::Date() {
    time(&rawtime);
}

Date::Date(int daysFromNow) {
    time(&rawtime);
    rawtime += daysFromNow * 24 * 3600;
}

string Date::toString() const {
    struct tm* timeinfo = localtime(&rawtime);
    char buffer[80];
    strftime(buffer, 80, "%Y-%m-%d", timeinfo);
    return string(buffer);
}

bool Date::operator<(const Date& other) const {
    return rawtime < other.rawtime;
}

bool Date::operator>(const Date& other) const {
    return rawtime > other.rawtime;
}

int Date::daysBetween(const Date& other) const {
    double seconds = difftime(rawtime, other.rawtime);
    return seconds / (24 * 3600);
}

// ==================== BOOK IMPLEMENTATION ====================
Book::Book(string id, string t, string a, string g, string isbn, int copies)
    : bookID(id), title(t), author(a), genre(g), ISBN(isbn), 
      totalCopies(copies), availableCopies(copies) {}

string Book::getBookID() const { return bookID; }
string Book::getTitle() const { return title; }
string Book::getAuthor() const { return author; }
string Book::getGenre() const { return genre; }
string Book::getISBN() const { return ISBN; }
int Book::getTotalCopies() const { return totalCopies; }
int Book::getAvailableCopies() const { return availableCopies; }

void Book::setTitle(const string& t) { title = t; }
void Book::setAuthor(const string& a) { author = a; }
void Book::setGenre(const string& g) { genre = g; }

bool Book::borrowCopy() {
    if (availableCopies > 0) {
        availableCopies--;
        return true;
    }
    return false;
}

bool Book::returnCopy() {
    if (availableCopies < totalCopies) {
        availableCopies++;
        return true;
    }
    return false;
}

void Book::addCopies(int count) {
    totalCopies += count;
    availableCopies += count;
}

bool Book::removeCopies(int count) {
    if (count <= totalCopies) {
        totalCopies -= count;
        availableCopies = min(availableCopies, totalCopies);
        return true;
    }
    return false;
}

void Book::display() const {
    cout << "\n[BOOK DETAILS]\n";
    cout << "ID: " << bookID << endl;
    cout << "Title: " << title << endl;
    cout << "Author: " << author << endl;
    cout << "Genre: " << genre << endl;
    cout << "ISBN: " << ISBN << endl;
    cout << "Copies: " << availableCopies << "/" << totalCopies << endl;
    cout << "Status: " << (availableCopies > 0 ? "Available" : "Checked Out") << endl;
    cout << string(40, '-') << endl;
}

// ==================== MEMBER IMPLEMENTATION ====================
Member::Member(string id, string n, string e)
    : memberID(id), name(n), email(e), joinDate(), fines(0.0) {}

string Member::getMemberID() const { return memberID; }
string Member::getName() const { return name; }
string Member::getEmail() const { return email; }
double Member::getFines() const { return fines; }
const Date& Member::getJoinDate() const { return joinDate; }

bool Member::borrowBook(const string& bookID, const Date& dueDate) {
    if (borrowedBooks.size() >= 5) {
        cout << "Borrowing limit reached (max 5 books)" << endl;
        return false;
    }
    borrowedBooks[bookID] = dueDate;
    return true;
}

bool Member::returnBook(const string& bookID) {
    auto it = borrowedBooks.find(bookID);
    if (it != borrowedBooks.end()) {
        borrowedBooks.erase(it);
        return true;
    }
    return false;
}

void Member::addFine(double amount) { fines += amount; }
void Member::payFine(double amount) { fines = max(0.0, fines - amount); }

bool Member::hasBook(const string& bookID) const {
    return borrowedBooks.find(bookID) != borrowedBooks.end();
}

Date Member::getDueDate(const string& bookID) const {
    auto it = borrowedBooks.find(bookID);
    if (it != borrowedBooks.end()) return it->second;
    return Date();
}

vector<string> Member::getBorrowedBooks() const {
    vector<string> books;
    for (const auto& pair : borrowedBooks) {
        books.push_back(pair.first);
    }
    return books;
}

int Member::getBorrowedCount() const { return borrowedBooks.size(); }

void Member::display() const {
    cout << "\n[MEMBER DETAILS]\n";
    cout << "Member ID: " << memberID << endl;
    cout << "Name: " << name << endl;
    cout << "Email: " << email << endl;
    cout << "Join Date: " << joinDate.toString() << endl;
    cout << "Fines: $" << fixed << setprecision(2) << fines << endl;
    cout << "Books Borrowed: " << borrowedBooks.size() << endl;
    
    if (!borrowedBooks.empty()) {
        cout << "Currently Borrowed:" << endl;
        for (const auto& pair : borrowedBooks) {
            cout << "  - Book ID: " << pair.first 
                 << " (Due: " << pair.second.toString() << ")" << endl;
        }
    }
    cout << string(40, '-') << endl;
}

// ==================== LIBRARY SYSTEM IMPLEMENTATION ====================
string LibraryManagementSystem::toLower(const string& str) {
    string lower = str;
    transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    return lower;
}

void LibraryManagementSystem::updateIndexes(const shared_ptr<Book>& book) {
    string lowerTitle = toLower(book->getTitle());
    string lowerAuthor = toLower(book->getAuthor());
    
    // Store bookID with title/author for quick lookup
    titleIndex[lowerTitle] = book->getBookID();
    authorIndex[lowerAuthor] = book->getBookID();
    genreIndex[book->getGenre()] = book->getBookID();
}

string LibraryManagementSystem::generateTransactionID() {
    static int counter = 1000;
    return "T" + to_string(counter++);
}

bool LibraryManagementSystem::addBook(const string& bookID, const string& title,
                                     const string& author, const string& genre,
                                     const string& ISBN, int copies) {
    
    if (books.find(bookID) != books.end()) {
        // Update existing book
        books[bookID]->addCopies(copies);
        return true;
    }
    
    auto newBook = make_shared<Book>(bookID, title, author, genre, ISBN, copies);
    books[bookID] = newBook;
    updateIndexes(newBook);
    return true;
}

bool LibraryManagementSystem::removeBook(const string& bookID, int copies) {
    auto it = books.find(bookID);
    if (it == books.end()) {
        cout << "Book not found: " << bookID << endl;
        return false;
    }
    
    auto& book = it->second;
    if (copies >= book->getTotalCopies()) {
        // Remove entire book
        books.erase(it);
        titleIndex.erase(toLower(book->getTitle()));
        authorIndex.erase(toLower(book->getAuthor()));
        genreIndex.erase(book->getGenre());
        cout << "Book removed: " << bookID << endl;
    } else {
        // Remove some copies
        if (book->removeCopies(copies)) {
            cout << "Removed " << copies << " copies from book: " << bookID << endl;
            return true;
        }
    }
    return true;
}

bool LibraryManagementSystem::updateBook(const string& bookID, const string& newTitle,
                                        const string& newAuthor, const string& newGenre) {
    auto it = books.find(bookID);
    if (it == books.end()) {
        cout << "Book not found: " << bookID << endl;
        return false;
    }
    
    auto& book = it->second;
    
    // Remove old indexes
    if (!newTitle.empty()) {
        titleIndex.erase(toLower(book->getTitle()));
        book->setTitle(newTitle);
    }
    if (!newAuthor.empty()) {
        authorIndex.erase(toLower(book->getAuthor()));
        book->setAuthor(newAuthor);
    }
    if (!newGenre.empty()) {
        genreIndex.erase(book->getGenre());
        book->setGenre(newGenre);
    }
    
    // Add new indexes
    updateIndexes(book);
    return true;
}

bool LibraryManagementSystem::registerMember(const string& memberID,
                                           const string& name, const string& email) {
    if (members.find(memberID) != members.end()) {
        cout << "Member ID already exists: " << memberID << endl;
        return false;
    }
    
    auto newMember = make_shared<Member>(memberID, name, email);
    members[memberID] = newMember;
    return true;
}

bool LibraryManagementSystem::removeMember(const string& memberID) {
    auto it = members.find(memberID);
    if (it == members.end()) {
        cout << "Member not found: " << memberID << endl;
        return false;
    }
    
    auto& member = it->second;
    if (member->getBorrowedCount() > 0) {
        cout << "Cannot remove member with borrowed books" << endl;
        return false;
    }
    
    if (member->getFines() > 0) {
        cout << "Cannot remove member with outstanding fines" << endl;
        return false;
    }
    
    members.erase(it);
    cout << "Member removed: " << memberID << endl;
    return true;
}

vector<shared_ptr<Book>> LibraryManagementSystem::searchByTitle(const string& title,
                                                              bool exactMatch) {
    vector<shared_ptr<Book>> results;
    string searchTerm = toLower(title);
    
    if (exactMatch) {
        auto it = titleIndex.find(searchTerm);
        if (it != titleIndex.end()) {
            auto bookIt = books.find(it->second);
            if (bookIt != books.end()) {
                results.push_back(bookIt->second);
            }
        }
    } else {
        // Partial match search
        for (const auto& entry : titleIndex) {
            if (entry.first.find(searchTerm) != string::npos) {
                auto bookIt = books.find(entry.second);
                if (bookIt != books.end()) {
                    results.push_back(bookIt->second);
                }
            }
        }
    }
    
    return results;
}

vector<shared_ptr<Book>> LibraryManagementSystem::searchByAuthor(const string& author) {
    vector<shared_ptr<Book>> results;
    string searchTerm = toLower(author);
    
    auto it = authorIndex.find(searchTerm);
    if (it != authorIndex.end()) {
        auto bookIt = books.find(it->second);
        if (bookIt != books.end()) {
            results.push_back(bookIt->second);
        }
    }
    
    return results;
}

vector<shared_ptr<Book>> LibraryManagementSystem::searchByGenre(const string& genre) {
    vector<shared_ptr<Book>> results;
    
    auto it = genreIndex.find(genre);
    if (it != genreIndex.end()) {
        auto bookIt = books.find(it->second);
        if (bookIt != books.end()) {
            results.push_back(bookIt->second);
        }
    }
    
    return results;
}

bool LibraryManagementSystem::borrowBook(const string& memberID, const string& bookID,
                                        int days) {
    // Validate member
    auto memberIt = members.find(memberID);
    if (memberIt == members.end()) {
        cout << "Member not found: " << memberID << endl;
        return false;
    }
    
    // Validate book
    auto bookIt = books.find(bookID);
    if (bookIt == books.end()) {
        cout << "Book not found: " << bookID << endl;
        return false;
    }
    
    auto& member = memberIt->second;
    auto& book = bookIt->second;
    
    // Check conditions
    if (book->getAvailableCopies() == 0) {
        cout << "Book not available: " << book->getTitle() << endl;
        return false;
    }
    
    if (member->getBorrowedCount() >= 5) {
        cout << "Member has reached borrowing limit (max 5 books)" << endl;
        return false;
    }
    
    if (member->getFines() > 10.0) {
        cout << "Member has outstanding fines: $" << member->getFines() << endl;
        return false;
    }
    
    // Process borrowing
    Date dueDate(days);
    if (!member->borrowBook(bookID, dueDate)) return false;
    if (!book->borrowCopy()) return false;
    
    // Add to due dates tracking
    dueDates.insert({dueDate, {bookID, memberID}});
    
    // Update popularity
    bookPopularity[bookID]++;
    
    return true;
}

bool LibraryManagementSystem::returnBook(const string& memberID, const string& bookID) {
    auto memberIt = members.find(memberID);
    auto bookIt = books.find(bookID);
    
    if (memberIt == members.end() || bookIt == books.end()) {
        cout << "Member or book not found" << endl;
        return false;
    }
    
    auto& member = memberIt->second;
    auto& book = bookIt->second;
    
    if (!member->hasBook(bookID)) {
        cout << "Member didn't borrow this book" << endl;
        return false;
    }
    
    // Check for overdue
    Date dueDate = member->getDueDate(bookID);
    Date returnDate;
    
    if (returnDate > dueDate) {
        int daysOverdue = dueDate.daysBetween(returnDate);
        double fine = daysOverdue * 0.5;
        member->addFine(fine);
        cout << "Book overdue by " << daysOverdue << " days. Fine: $" 
             << fixed << setprecision(2) << fine << endl;
    }
    
    // Process return
    member->returnBook(bookID);
    book->returnCopy();
    
    return true;
}

vector<pair<string, string>> LibraryManagementSystem::checkOverdueBooks() const {
    vector<pair<string, string>> overdue;
    Date currentDate;
    
    for (const auto& entry : dueDates) {
        if (currentDate > entry.first) {
            overdue.push_back(entry.second);
        }
    }
    
    return overdue;
}

void LibraryManagementSystem::displayAllBooks() const {
    cout << "\n=== ALL BOOKS IN LIBRARY ===\n";
    if (books.empty()) {
        cout << "No books in library.\n";
        return;
    }
    
    for (const auto& pair : books) {
        pair.second->display();
    }
}

void LibraryManagementSystem::displayAllMembers() const {
    cout << "\n=== ALL MEMBERS ===\n";
    if (members.empty()) {
        cout << "No members registered.\n";
        return;
    }
    
    for (const auto& pair : members) {
        pair.second->display();
    }
}

void LibraryManagementSystem::displayStatistics() const {
    int totalBooks = 0;
    int availableBooks = 0;
    
    for (const auto& pair : books) {
        totalBooks += pair.second->getTotalCopies();
        availableBooks += pair.second->getAvailableCopies();
    }
    
    int activeMembers = 0;
    for (const auto& pair : members) {
        if (pair.second->getBorrowedCount() > 0) {
            activeMembers++;
        }
    }
    
    cout << "\n=== LIBRARY STATISTICS ===\n";
    cout << "Total Books: " << totalBooks << endl;
    cout << "Available Books: " << availableBooks << endl;
    cout << "Borrowed Books: " << (totalBooks - availableBooks) << endl;
    cout << "Total Members: " << members.size() << endl;
    cout << "Active Members: " << activeMembers << endl;
    
    auto overdue = checkOverdueBooks();
    cout << "Overdue Books: " << overdue.size() << endl;
    
    // Find most popular book
    if (!bookPopularity.empty()) {
        auto maxElem = max_element(bookPopularity.begin(), bookPopularity.end(),
            [](const pair<string, int>& a, const pair<string, int>& b) {
                return a.second < b.second;
            });
        
        auto bookIt = books.find(maxElem->first);
        if (bookIt != books.end()) {
            cout << "Most Popular Book: " << bookIt->second->getTitle()
                 << " (" << maxElem->second << " borrows)" << endl;
        }
    }
}

bool LibraryManagementSystem::saveToFile(const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cout << "Cannot open file: " << filename << endl;
        return false;
    }
    
    // Save books
    file << "[BOOKS]\n";
    for (const auto& pair : books) {
        auto& book = pair.second;
        file << book->getBookID() << ","
             << book->getTitle() << ","
             << book->getAuthor() << ","
             << book->getGenre() << ","
             << book->getISBN() << ","
             << book->getTotalCopies() << ","
             << book->getAvailableCopies() << "\n";
    }
    
    // Save members
    file << "\n[MEMBERS]\n";
    for (const auto& pair : members) {
        auto& member = pair.second;
        file << member->getMemberID() << ","
             << member->getName() << ","
             << member->getEmail() << ","
             << member->getJoinDate().toString() << ","
             << fixed << setprecision(2) << member->getFines() << "\n";
    }
    
    file.close();
    return true;
}

bool LibraryManagementSystem::loadFromFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Cannot open file: " << filename << endl;
        return false;
    }
    
    string line;
    string section;
    
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        if (line == "[BOOKS]") {
            section = "BOOKS";
            continue;
        } else if (line == "[MEMBERS]") {
            section = "MEMBERS";
            continue;
        }
        
        if (section == "BOOKS") {
            // Parse book data
            istringstream iss(line);
            vector<string> tokens;
            string token;
            
            while (getline(iss, token, ',')) {
                tokens.push_back(token);
            }
            
            if (tokens.size() >= 7) {
                int copies = stoi(tokens[5]);
                int available = stoi(tokens[6]);
                
                // Create book
                auto book = make_shared<Book>(tokens[0], tokens[1], tokens[2], 
                                             tokens[3], tokens[4], copies);
                // Set correct available copies
                for (int i = 0; i < copies - available; i++) {
                    book->borrowCopy();
                }
                
                books[tokens[0]] = book;
                updateIndexes(book);
            }
        } else if (section == "MEMBERS") {
            // Parse member data (simplified)
            istringstream iss(line);
            vector<string> tokens;
            string token;
            
            while (getline(iss, token, ',')) {
                tokens.push_back(token);
            }
            
            if (tokens.size() >= 3) {
                registerMember(tokens[0], tokens[1], tokens[2]);
            }
        }
    }
    
    file.close();
    return true;
}
