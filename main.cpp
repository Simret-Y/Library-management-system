#include "LibrarySystem.h"
#include <iostream>

using namespace std;

void displayMenu() {
    cout << "\n=== LIBRARY MANAGEMENT SYSTEM ===\n";
    cout << "1. Add Book\n";
    cout << "2. Remove Book\n";
    cout << "3. Update Book\n";
    cout << "4. Register Member\n";
    cout << "5. Remove Member\n";
    cout << "6. Search Books\n";
    cout << "7. Borrow Book\n";
    cout << "8. Return Book\n";
    cout << "9. Display All Books\n";
    cout << "10. Display All Members\n";
    cout << "11. Display Statistics\n";
    cout << "12. Check Overdue Books\n";
    cout << "13. Save to File\n";
    cout << "14. Load from File\n";
    cout << "15. Demo Mode\n";
    cout << "16. Exit\n";
    cout << "Enter your choice (1-16): ";
}

void runDemo() {
    cout << "\n=== RUNNING DEMONSTRATION ===\n";
    
    LibraryManagementSystem library;
    
    // Add books
    library.addBook("B001", "The Great Gatsby", "F. Scott Fitzgerald", 
                    "Fiction", "9780743273565", 3);
    library.addBook("B002", "To Kill a Mockingbird", "Harper Lee", 
                    "Fiction", "9780061120084", 2);
    library.addBook("B003", "1984", "George Orwell", 
                    "Dystopian", "9780451524935", 4);
    
    // Register members
    library.registerMember("M001", "Alice Johnson", "alice@email.com");
    library.registerMember("M002", "Bob Smith", "bob@email.com");
    
    // Borrow books
    library.borrowBook("M001", "B001", 14);
    library.borrowBook("M002", "B003", 7);
    
    // Display results
    library.displayAllBooks();
    library.displayAllMembers();
    library.displayStatistics();
    
    cout << "\n=== DEMONSTRATION COMPLETE ===\n";
}

int main() {
    LibraryManagementSystem library;
    int choice;
    bool running = true;
    
    cout << "===================================\n";
    cout << "   LIBRARY MANAGEMENT SYSTEM\n";
    cout << "===================================\n";
    
    while (running) {
        displayMenu();
        cin >> choice;
        cin.ignore(); // Clear newline
        
        switch (choice) {
            case 1: { // Add Book
                string id, title, author, genre, isbn;
                int copies;
                
                cout << "\n=== ADD NEW BOOK ===\n";
                cout << "Book ID: "; getline(cin, id);
                cout << "Title: "; getline(cin, title);
                cout << "Author: "; getline(cin, author);
                cout << "Genre: "; getline(cin, genre);
                cout << "ISBN: "; getline(cin, isbn);
                cout << "Number of copies: "; cin >> copies;
                cin.ignore();
                
                if (library.addBook(id, title, author, genre, isbn, copies)) {
                    cout << "✓ Book added successfully!\n";
                } else {
                    cout << "✗ Failed to add book.\n";
                }
                break;
            }
            
            case 2: { // Remove Book
                string id;
                int copies;
                
                cout << "\n=== REMOVE BOOK ===\n";
                cout << "Book ID: "; getline(cin, id);
                cout << "Number of copies to remove (0 to remove all): "; 
                cin >> copies;
                cin.ignore();
                
                if (copies == 0) {
                    cout << "Remove entire book entry? (y/n): ";
                    char confirm;
                    cin >> confirm;
                    cin.ignore();
                    if (confirm == 'y' || confirm == 'Y') {
                        if (library.removeBook(id, 9999)) { // Large number to remove all
                            cout << "✓ Book removed successfully!\n";
                        } else {
                            cout << "✗ Failed to remove book.\n";
                        }
                    }
                } else {
                    if (library.removeBook(id, copies)) {
                        cout << "✓ " << copies << " copies removed!\n";
                    } else {
                        cout << "✗ Failed to remove copies.\n";
                    }
                }
                break;
            }
            
            case 4: { // Register Member
                string id, name, email;
                
                cout << "\n=== REGISTER NEW MEMBER ===\n";
                cout << "Member ID: "; getline(cin, id);
                cout << "Full Name: "; getline(cin, name);
                cout << "Email: "; getline(cin, email);
                
                if (library.registerMember(id, name, email)) {
                    cout << "✓ Member registered successfully!\n";
                } else {
                    cout << "✗ Member ID already exists.\n";
                }
                break;
            }
            
            case 6: { // Search Books
                int searchChoice;
                cout << "\n=== SEARCH BOOKS ===\n";
                cout << "1. Search by Title\n";
                cout << "2. Search by Author\n";
                cout << "3. Search by Genre\n";
                cout << "Choice: ";
                cin >> searchChoice;
                cin.ignore();
                
                if (searchChoice == 1) {
                    string title;
                    cout << "Enter title: "; getline(cin, title);
                    auto results = library.searchByTitle(title);
                    cout << "\nFound " << results.size() << " book(s):\n";
                    for (const auto& book : results) {
                        cout << "- ID: " << book->getBookID() 
                             << ", Title: " << book->getTitle()
                             << ", Author: " << book->getAuthor()
                             << ", Available: " << book->getAvailableCopies() << "/" 
                             << book->getTotalCopies() << endl;
                    }
                } else if (searchChoice == 2) {
                    string author;
                    cout << "Enter author: "; getline(cin, author);
                    auto results = library.searchByAuthor(author);
                    cout << "\nFound " << results.size() << " book(s):\n";
                    for (const auto& book : results) {
                        cout << "- " << book->getTitle() << endl;
                    }
                } else if (searchChoice == 3) {
                    string genre;
                    cout << "Enter genre: "; getline(cin, genre);
                    auto results = library.searchByGenre(genre);
                    cout << "\nFound " << results.size() << " book(s):\n";
                    for (const auto& book : results) {
                        cout << "- " << book->getTitle() << " by " << book->getAuthor() << endl;
                    }
                }
                break;
            }
            
            case 7: { // Borrow Book
                string memberID, bookID;
                int days;
                
                cout << "\n=== BORROW BOOK ===\n";
                cout << "Member ID: "; getline(cin, memberID);
                cout << "Book ID: "; getline(cin, bookID);
                cout << "Days to borrow (default 14): "; 
                cin >> days;
                cin.ignore();
                
                if (days <= 0) days = 14;
                
                if (library.borrowBook(memberID, bookID, days)) {
                    cout << "✓ Book borrowed successfully!\n";
                }
                break;
            }
            
            case 8: { // Return Book
                string memberID, bookID;
                
                cout << "\n=== RETURN BOOK ===\n";
                cout << "Member ID: "; getline(cin, memberID);
                cout << "Book ID: "; getline(cin, bookID);
                
                if (library.returnBook(memberID, bookID)) {
                    cout << "✓ Book returned successfully!\n";
                } else {
                    cout << "✗ Failed to return book.\n";
                }
                break;
            }
            
            case 9: // Display All Books
                library.displayAllBooks();
                break;
                
            case 10: // Display All Members
                library.displayAllMembers();
                break;
                
            case 11: // Display Statistics
                library.displayStatistics();
                break;
                
            case 12: { // Check Overdue Books
                auto overdue = library.checkOverdueBooks();
                if (overdue.empty()) {
                    cout << "\nNo overdue books!\n";
                } else {
                    cout << "\n=== OVERDUE BOOKS ===\n";
                    cout << "Found " << overdue.size() << " overdue book(s):\n";
                    for (const auto& pair : overdue) {
                        cout << "- Book ID: " << pair.first 
                             << ", Member ID: " << pair.second << endl;
                    }
                }
                break;
            }
            
            case 13: // Save to File
                if (library.saveToFile("library_data.txt")) {
                    cout << "✓ Data saved to library_data.txt\n";
                } else {
                    cout << "✗ Failed to save data.\n";
                }
                break;
                
            case 14: // Load from File
                if (library.loadFromFile("library_data.txt")) {
                    cout << "✓ Data loaded from library_data.txt\n";
                } else {
                    cout << "✗ Failed to load data.\n";
                }
                break;
                
            case 15: // Demo Mode
                runDemo();
                break;
                
            case 16: // Exit
                running = false;
                cout << "\nThank you for using the Library Management System!\n";
                break;
                
            default:
                cout << "Invalid choice. Please try again.\n";
        }
        
        cout << "\nPress Enter to continue...";
        cin.ignore();
    }
    
    return 0;
}