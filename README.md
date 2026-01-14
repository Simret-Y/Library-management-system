# Library Management System

## Description
This project implements a Library Management System in C++.  
It is a backend-focused console application with a frontend in HTML/CSS/JSON.  
The system manages a library's book catalog, member registrations, borrowing rules, and fines.

## Features
- Inventory Management: Add, update, remove books; track available copies  
- Member Management: Register users and track borrowing history & fines  
- Borrowing Logic: Enforces rules (max 5 books, checks fines)  
- Search Engine: Fast search by Title, Author, or Genre  
- Financial Tracking: Calculates fines for late returns ($0.50/day)  
- Data Persistence: Save/load library state to `library_data.txt`  

## File Breakdown
- `LibrarySystem.h` → Class structure and function definitions  
- `LibrarySystem.cpp` → Main logic engine  
- `main.cpp` → Console-based user interface  
- `/frontend` → HTML, CSS, JSON files  

## Technologies Used
- C++ (OOP, STL, file I/O)  
- HTML, CSS, JSON  
- Git & GitHub  

## What I Learned
- Object-Oriented Programming & memory management  
- Data structures: std::map, std::multimap, std::vector  
- File parsing and serialization  
- Algorithm design: search logic, date calculations  
- Basic frontend-backend integration  
- Working collaboratively in a group project

