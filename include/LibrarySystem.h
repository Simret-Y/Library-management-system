
#ifndef LIBRARY_SYSTEM_H
#define LIBRARY_SYSTEM_H

#include <string>
#include <vector>
#include <memory>
#include <map>

// Forward declarations
class Date;
class Book;
class Member;
class Transaction;
class LibraryManagementSystem;

// Date class
class Date {
private:
    time_t rawtime;
    
public:
    Date();
    Date(int daysFromNow);
    std::string toString() const;
    bool operator<(const Date& other) const;
    bool operator>(const Date& other) const;
    int daysBetween(const Date& other) const;
};

// Book class
class Book {
private:
    std::string bookID;
    std::string title;
    std::string author;
    std::string genre;
    std::string ISBN;
    int totalCopies;
    int availableCopies;
    
public:
    Book(std::string id, std::string t, std::string a, 
         std::string g, std::string isbn, int copies = 1);
    
    // Getters
    std::string getBookID() const;
    std::string getTitle() const;
    std::string getAuthor() const;
    std::string getGenre() const;
    std::string getISBN() const;
    int getTotalCopies() const;
    int getAvailableCopies() const;
    
    // Setters
    void setTitle(const std::string& t);
    void setAuthor(const std::string& a);
    void setGenre(const std::string& g);
    
    // Copy management
    bool borrowCopy();
    bool returnCopy();
    void addCopies(int count);
    bool removeCopies(int count);
    
    void display() const;
};

// Member class
class Member {
private:
    std::string memberID;
    std::string name;
    std::string email;
    Date joinDate;
    double fines;
    std::map<std::string, Date> borrowedBooks;
    
public:
    Member(std::string id, std::string n, std::string e);
    
    // Getters
    std::string getMemberID() const;
    std::string getName() const;
    std::string getEmail() const;
    double getFines() const;
    const Date& getJoinDate() const;
    
    // Borrowing management
    bool borrowBook(const std::string& bookID, const Date& dueDate);
    bool returnBook(const std::string& bookID);
    void addFine(double amount);
    void payFine(double amount);
    bool hasBook(const std::string& bookID) const;
    Date getDueDate(const std::string& bookID) const;
    std::vector<std::string> getBorrowedBooks() const;
    int getBorrowedCount() const;
    
    void display() const;
};

// Main Library System
class LibraryManagementSystem {
private:
    std::map<std::string, std::shared_ptr<Book>> books;
    std::map<std::string, std::shared_ptr<Member>> members;
    std::map<std::string, std::string> titleIndex;
    std::map<std::string, std::string> authorIndex;
    std::map<std::string, std::string> genreIndex;
    std::multimap<Date, std::pair<std::string, std::string>> dueDates;
    std::map<std::string, int> bookPopularity;
    
    std::string toLower(const std::string& str);
    void updateIndexes(const std::shared_ptr<Book>& book);
    std::string generateTransactionID();
    
public:
    // Book management
    bool addBook(const std::string& bookID, const std::string& title,
                 const std::string& author, const std::string& genre,
                 const std::string& ISBN, int copies = 1);
    bool removeBook(const std::string& bookID, int copies = 1);
    bool updateBook(const std::string& bookID, const std::string& newTitle = "",
                   const std::string& newAuthor = "", const std::string& newGenre = "");
    
    // Member management
    bool registerMember(const std::string& memberID, 
                       const std::string& name, const std::string& email);
    bool removeMember(const std::string& memberID);
    
    // Search operations
    std::vector<std::shared_ptr<Book>> searchByTitle(const std::string& title, 
                                                    bool exactMatch = false);
    std::vector<std::shared_ptr<Book>> searchByAuthor(const std::string& author);
    std::vector<std::shared_ptr<Book>> searchByGenre(const std::string& genre);
    
    // Borrow/return operations
    bool borrowBook(const std::string& memberID, const std::string& bookID, 
                   int days = 14);
    bool returnBook(const std::string& memberID, const std::string& bookID);
    
    // Utility methods
    std::vector<std::pair<std::string, std::string>> checkOverdueBooks() const;
    void displayAllBooks() const;
    void displayAllMembers() const;
    void displayStatistics() const;
    bool saveToFile(const std::string& filename);
    bool loadFromFile(const std::string& filename);
};

#endif
