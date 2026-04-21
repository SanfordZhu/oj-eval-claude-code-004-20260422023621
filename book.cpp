#include "book.hpp"
#include "utils.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>

BookManager::BookManager(const std::string& filename) : bookFile(filename) {
    loadBooks();
}

BookManager::~BookManager() {
    saveBooks();
}

bool BookManager::loadBooks() {
    std::ifstream file(bookFile, std::ios::binary);
    if (!file.is_open()) return false;

    books.clear();
    size_t count;
    file.read(reinterpret_cast<char*>(&count), sizeof(count));

    for (size_t i = 0; i < count; ++i) {
        Book book;
        size_t len;

        file.read(reinterpret_cast<char*>(&len), sizeof(len));
        book.isbn.resize(len);
        file.read(&book.isbn[0], len);

        file.read(reinterpret_cast<char*>(&len), sizeof(len));
        book.name.resize(len);
        file.read(&book.name[0], len);

        file.read(reinterpret_cast<char*>(&len), sizeof(len));
        book.author.resize(len);
        file.read(&book.author[0], len);

        file.read(reinterpret_cast<char*>(&len), sizeof(len));
        book.keyword.resize(len);
        file.read(&book.keyword[0], len);

        file.read(reinterpret_cast<char*>(&book.price), sizeof(book.price));
        file.read(reinterpret_cast<char*>(&book.quantity), sizeof(book.quantity));

        books.push_back(book);
    }

    file.close();
    return true;
}

bool BookManager::saveBooks() {
    std::ofstream file(bookFile, std::ios::binary);
    if (!file.is_open()) return false;

    size_t count = books.size();
    file.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (const auto& book : books) {
        size_t len = book.isbn.length();
        file.write(reinterpret_cast<const char*>(&len), sizeof(len));
        file.write(book.isbn.c_str(), len);

        len = book.name.length();
        file.write(reinterpret_cast<const char*>(&len), sizeof(len));
        file.write(book.name.c_str(), len);

        len = book.author.length();
        file.write(reinterpret_cast<const char*>(&len), sizeof(len));
        file.write(book.author.c_str(), len);

        len = book.keyword.length();
        file.write(reinterpret_cast<const char*>(&len), sizeof(len));
        file.write(book.keyword.c_str(), len);

        file.write(reinterpret_cast<const char*>(&book.price), sizeof(book.price));
        file.write(reinterpret_cast<const char*>(&book.quantity), sizeof(book.quantity));
    }

    file.close();
    return true;
}

int BookManager::findBook(const std::string& isbn) {
    for (size_t i = 0; i < books.size(); ++i) {
        if (books[i].isbn == isbn) {
            return i;
        }
    }
    return -1;
}

std::vector<std::string> BookManager::splitKeyword(const std::string& keyword) {
    return split(keyword, '|');
}

std::string BookManager::joinKeyword(const std::vector<std::string>& keywords) {
    std::string result;
    for (size_t i = 0; i < keywords.size(); ++i) {
        if (i > 0) result += "|";
        result += keywords[i];
    }
    return result;
}

std::vector<Book> BookManager::showBooks(const std::string& type, const std::string& value) {
    std::vector<Book> result;

    if (type.empty()) {
        result = books;
    } else if (type == "-ISBN") {
        for (const auto& book : books) {
            if (book.isbn == value) {
                result.push_back(book);
            }
        }
    } else if (type == "-name") {
        for (const auto& book : books) {
            if (book.name == value) {
                result.push_back(book);
            }
        }
    } else if (type == "-author") {
        for (const auto& book : books) {
            if (book.author == value) {
                result.push_back(book);
            }
        }
    } else if (type == "-keyword") {
        std::vector<std::string> searchKeywords = splitKeyword(value);
        for (const auto& book : books) {
            std::vector<std::string> bookKeywords = splitKeyword(book.keyword);
            bool match = true;
            for (const auto& kw : searchKeywords) {
                if (std::find(bookKeywords.begin(), bookKeywords.end(), kw) == bookKeywords.end()) {
                    match = false;
                    break;
                }
            }
            if (match) {
                result.push_back(book);
            }
        }
    }

    std::sort(result.begin(), result.end(), [](const Book& a, const Book& b) {
        return a.isbn < b.isbn;
    });

    return result;
}

bool BookManager::buyBook(const std::string& isbn, int quantity, double& totalCost) {
    if (!isValidISBN(isbn) || quantity <= 0) return false;

    int index = findBook(isbn);
    if (index == -1) return false;

    if (books[index].quantity < quantity) return false;

    totalCost = books[index].price * quantity;
    books[index].quantity -= quantity;
    saveBooks();
    return true;
}

bool BookManager::selectBook(const std::string& isbn) {
    if (!isValidISBN(isbn)) return false;

    int index = findBook(isbn);
    if (index == -1) {
        Book newBook(isbn, "", "", "", 0.0, 0);
        books.push_back(newBook);
        saveBooks();
    }

    selectedISBN = isbn;
    return true;
}

bool BookManager::modifyBook(const std::string& isbn, const std::string& name, const std::string& author, const std::string& keyword, double price) {
    if (selectedISBN.empty()) return false;

    int index = findBook(selectedISBN);
    if (index == -1) return false;

    if (!isbn.empty()) {
        if (isbn == selectedISBN) return false;
        if (findBook(isbn) != -1) return false;
        books[index].isbn = isbn;
        selectedISBN = isbn;
    }

    if (!name.empty()) {
        if (!isValidBookName(name)) return false;
        books[index].name = name;
    }

    if (!author.empty()) {
        if (!isValidAuthor(author)) return false;
        books[index].author = author;
    }

    if (!keyword.empty()) {
        if (!isValidKeyword(keyword)) return false;
        std::vector<std::string> keywords = splitKeyword(keyword);
        std::sort(keywords.begin(), keywords.end());
        for (size_t i = 1; i < keywords.size(); ++i) {
            if (keywords[i] == keywords[i-1]) return false;
        }
        books[index].keyword = keyword;
    }

    if (price > 0) {
        books[index].price = price;
    }

    saveBooks();
    return true;
}

bool BookManager::importBook(int quantity, double totalCost) {
    if (selectedISBN.empty()) return false;
    if (quantity <= 0 || totalCost <= 0) return false;

    int index = findBook(selectedISBN);
    if (index == -1) return false;

    books[index].quantity += quantity;
    saveBooks();
    return true;
}