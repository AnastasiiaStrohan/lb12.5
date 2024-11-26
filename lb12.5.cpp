#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <windows.h>

struct Product {
    char name[50]; // Фіксована довжина рядка
    double price;
    double utility;
};

struct Consumer {
    char name[50]; // Фіксована довжина рядка
    double income;
};

struct Record {
    Consumer consumer;
    Product product;
};

// Функція для запису запису у бінарний файл
void writeRecordToFile(const std::string& filename, const Record& record, bool append = true) {
    std::ofstream file(filename, append ? std::ios::binary | std::ios::app : std::ios::binary | std::ios::trunc);
    if (!file) {
        std::cerr << "Помилка відкриття файлу.\n";
        return;
    }
    file.write(reinterpret_cast<const char*>(&record), sizeof(record));
    file.close();
}

// Функція для читання всіх записів з бінарного файлу
void readRecordsFromFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Файл не знайдено або не вдалося відкрити.\n";
        return;
    }
    Record record;
    while (file.read(reinterpret_cast<char*>(&record), sizeof(record))) {
        std::cout << "Споживач: " << record.consumer.name << ", Прибуток: " << record.consumer.income << "\n"
            << "Продукт: " << record.product.name << ", Ціна: " << record.product.price
            << ", Корисність: " << record.product.utility << "\n";
    }
    file.close();
}

// Функція для заміни запису у бінарному файлі
void replaceRecordInFile(const std::string& filename, int index, const Record& newRecord) {
    std::fstream file(filename, std::ios::binary | std::ios::in | std::ios::out);
    if (!file) {
        std::cerr << "Файл не знайдено або не вдалося відкрити.\n";
        return;
    }
    file.seekp(index * sizeof(Record), std::ios::beg); // Перехід до потрібного запису
    file.write(reinterpret_cast<const char*>(&newRecord), sizeof(newRecord));
    file.close();
}

// Функція для видалення запису у бінарному файлі
void deleteRecordFromFile(const std::string& filename, int index) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Файл не знайдено або не вдалося відкрити.\n";
        return;
    }
    std::ofstream tempFile("temp.dat", std::ios::binary);
    Record record;
    int currentIndex = 0;
    while (file.read(reinterpret_cast<char*>(&record), sizeof(record))) {
        if (currentIndex != index) {
            tempFile.write(reinterpret_cast<const char*>(&record), sizeof(record));
        }
        currentIndex++;
    }
    file.close();
    tempFile.close();
    std::remove(filename.c_str());
    std::rename("temp.dat", filename.c_str());
}

// Запит 1: Продукти заданого рівня корисності, які може придбати споживач при заданому прибутку
void findAffordableProducts(const std::string& filename, double income, double minUtility) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Файл не знайдено або не вдалося відкрити.\n";
        return;
    }
    Record record;
    bool found = false;
    while (file.read(reinterpret_cast<char*>(&record), sizeof(record))) {
        if (record.product.utility >= minUtility && record.product.price <= income) {
            std::cout << "Продукт: " << record.product.name << ", Ціна: " << record.product.price
                << ", Корисність: " << record.product.utility << "\n";
            found = true;
        }
    }
    if (!found) {
        std::cout << "Немає доступних продуктів із заданими параметрами.\n";
    }
    file.close();
}

// Запит 2: Сумарна вартість продуктів із максимальним рівнем корисності
void calculateMaxUtilityCost(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Файл не знайдено або не вдалося відкрити.\n";
        return;
    }
    Record record;
    double maxUtility = 0;
    double totalCost = 0;
    while (file.read(reinterpret_cast<char*>(&record), sizeof(record))) {
        if (record.product.utility > maxUtility) {
            maxUtility = record.product.utility;
            totalCost = record.product.price;
        }
        else if (record.product.utility == maxUtility) {
            totalCost += record.product.price;
        }
    }
    std::cout << "Сумарна вартість продуктів із максимальною корисністю (" << maxUtility << "): " << totalCost << "\n";
    file.close();
}

// Запит 3: Споживачі, прибутку яких не вистачає для придбання продуктів за заданою ціною і рівнем корисності
void findConsumersBelowPrice(const std::string& filename, double price, double utility) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Файл не знайдено або не вдалося відкрити.\n";
        return;
    }
    Record record;
    bool found = false;
    while (file.read(reinterpret_cast<char*>(&record), sizeof(record))) {
        if (record.product.price >= price && record.product.utility >= utility && record.consumer.income < price) {
            std::cout << "Споживач: " << record.consumer.name << ", Прибуток: " << record.consumer.income << "\n";
            found = true;
        }
    }
    if (!found) {
        std::cout << "Немає таких споживачів.\n";
    }
    file.close();
}

// Основне меню
void displayMenu() {
    std::cout << "\nМеню:\n"
        << "1. Додати запис\n"
        << "2. Переглянути всі записи\n"
        << "3. Замінити запис\n"
        << "4. Видалити запис\n"
        << "5. Знайти доступні продукти\n"
        << "6. Порахувати сумарну вартість продуктів з максимальною корисністю\n"
        << "7. Знайти споживачів із недостатнім прибутком\n"
        << "8. Вихід\n"
        << "Ваш вибір: ";
}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    std::string filename;
    std::cout << "Введіть ім'я файлу: ";
    std::cin >> filename;

    int choice;
    do {
        displayMenu();
        std::cin >> choice;
        switch (choice) {
        case 1: {
            Record record;
            std::cout << "Введіть дані (ім'я споживача, прибуток, назва продукту, ціна, корисність): ";
            std::cin.ignore(); // Для коректного зчитування імен
            std::cin.getline(record.consumer.name, 50);
            std::cin >> record.consumer.income;
            std::cin.ignore();
            std::cin.getline(record.product.name, 50);
            std::cin >> record.product.price >> record.product.utility;
            writeRecordToFile(filename, record);
            break;
        }
        case 2:
            readRecordsFromFile(filename);
            break;
        case 3: {
            int index;
            std::cout << "Введіть індекс запису для заміни: ";
            std::cin >> index;
            Record record;
            std::cout << "Введіть нові дані (ім'я споживача, прибуток, назва продукту, ціна, корисність): ";
            std::cin.ignore();
            std::cin.getline(record.consumer.name, 50);
            std::cin >> record.consumer.income;
            std::cin.ignore();
            std::cin.getline(record.product.name, 50);
            std::cin >> record.product.price >> record.product.utility;
            replaceRecordInFile(filename, index, record);
            break;
        }
        case 4: {
            int index;
            std::cout << "Введіть індекс запису для видалення: ";
            std::cin >> index;
            deleteRecordFromFile(filename, index);
            break;
        }
        case 5: {
            double income, minUtility;
            std::cout << "Введіть прибуток і мінімальну корисність: ";
            std::cin >> income >> minUtility;
            findAffordableProducts(filename, income, minUtility);
            break;
        }
        case 6:
            calculateMaxUtilityCost(filename);
            break;
        case 7: {
            double price, utility;
            std::cout << "Введіть ціну і рівень корисності: ";
            std::cin >> price >> utility;
            findConsumersBelowPrice(filename, price, utility);
            break;
        }
        case 8:
            std::cout << "Вихід.\n";
            break;
        default:
            std::cerr << "Некоректний вибір. Спробуйте ще раз.\n";
        }
    } while (choice != 8);

    return 0;
}
