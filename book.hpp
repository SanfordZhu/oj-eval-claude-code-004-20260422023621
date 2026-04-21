#ifndef BOOK_HPP
#define BOOK_HPP

#include <string>
#include <vector>
#include <fstream>

struct Book {
    std::string isbn;
    std::string name;
    std::string author;
    std::string keyword;
    double price;
    int quantity;

    Book() : price(0.0), quantity(0) {}
    Book(const std::string& i, const std::string& n, const std::string& a, const std::string& k, double p, int q)
        : isbn(i), name(n), author(a), keyword(k), price(p), quantity(q) {}
};

class BookManager {
private:
    std::vector<Book> books;
    std::string selectedISBN;
    std::string bookFile;

    bool loadBooks();
    bool saveBooks();
    int findBook(const std::string& isbn);
    std::vector<std::string> splitKeyword(const std::string& keyword);
    std::string joinKeyword(const std::vector<std::string>& keywords);

public:
    BookManager(const std::string& filename = "books.dat");
    ~BookManager();

    std::vector<Book> showBooks(const std::string& type = "", const std::string& value = "");
    bool buyBook(const std::string& isbn, int quantity, double& totalCost);
    bool selectBook(const std::string& isbn);
    bool modifyBook(const std::string& isbn, const std::string& name, const std::string& author, const std::string& keyword, double price);
    bool importBook(int quantity, double totalCost);

    std::string getSelectedBook() const { return selectedISBN; }
    bool hasSelectedBook() const { return !selectedISBN.empty(); }
    void clearSelection() { selectedISBN.clear(); }
};

#endif