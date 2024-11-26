#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <windows.h>

struct Product {
    char name[50]; // Գ������� ������� �����
    double price;
    double utility;
};

struct Consumer {
    char name[50]; // Գ������� ������� �����
    double income;
};

struct Record {
    Consumer consumer;
    Product product;
};

// ������� ��� ������ ������ � ������� ����
void writeRecordToFile(const std::string& filename, const Record& record, bool append = true) {
    std::ofstream file(filename, append ? std::ios::binary | std::ios::app : std::ios::binary | std::ios::trunc);
    if (!file) {
        std::cerr << "������� �������� �����.\n";
        return;
    }
    file.write(reinterpret_cast<const char*>(&record), sizeof(record));
    file.close();
}

// ������� ��� ������� ��� ������ � �������� �����
void readRecordsFromFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "���� �� �������� ��� �� ������� �������.\n";
        return;
    }
    Record record;
    while (file.read(reinterpret_cast<char*>(&record), sizeof(record))) {
        std::cout << "��������: " << record.consumer.name << ", ��������: " << record.consumer.income << "\n"
            << "�������: " << record.product.name << ", ֳ��: " << record.product.price
            << ", ���������: " << record.product.utility << "\n";
    }
    file.close();
}

// ������� ��� ����� ������ � �������� ����
void replaceRecordInFile(const std::string& filename, int index, const Record& newRecord) {
    std::fstream file(filename, std::ios::binary | std::ios::in | std::ios::out);
    if (!file) {
        std::cerr << "���� �� �������� ��� �� ������� �������.\n";
        return;
    }
    file.seekp(index * sizeof(Record), std::ios::beg); // ������� �� ��������� ������
    file.write(reinterpret_cast<const char*>(&newRecord), sizeof(newRecord));
    file.close();
}

// ������� ��� ��������� ������ � �������� ����
void deleteRecordFromFile(const std::string& filename, int index) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "���� �� �������� ��� �� ������� �������.\n";
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

// ����� 1: �������� �������� ���� ���������, �� ���� �������� �������� ��� �������� ��������
void findAffordableProducts(const std::string& filename, double income, double minUtility) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "���� �� �������� ��� �� ������� �������.\n";
        return;
    }
    Record record;
    bool found = false;
    while (file.read(reinterpret_cast<char*>(&record), sizeof(record))) {
        if (record.product.utility >= minUtility && record.product.price <= income) {
            std::cout << "�������: " << record.product.name << ", ֳ��: " << record.product.price
                << ", ���������: " << record.product.utility << "\n";
            found = true;
        }
    }
    if (!found) {
        std::cout << "���� ��������� �������� �� �������� �����������.\n";
    }
    file.close();
}

// ����� 2: ������� ������� �������� �� ������������ ����� ���������
void calculateMaxUtilityCost(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "���� �� �������� ��� �� ������� �������.\n";
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
    std::cout << "������� ������� �������� �� ������������ ��������� (" << maxUtility << "): " << totalCost << "\n";
    file.close();
}

// ����� 3: ���������, �������� ���� �� ������� ��� ��������� �������� �� ������� ����� � ����� ���������
void findConsumersBelowPrice(const std::string& filename, double price, double utility) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "���� �� �������� ��� �� ������� �������.\n";
        return;
    }
    Record record;
    bool found = false;
    while (file.read(reinterpret_cast<char*>(&record), sizeof(record))) {
        if (record.product.price >= price && record.product.utility >= utility && record.consumer.income < price) {
            std::cout << "��������: " << record.consumer.name << ", ��������: " << record.consumer.income << "\n";
            found = true;
        }
    }
    if (!found) {
        std::cout << "���� ����� ����������.\n";
    }
    file.close();
}

// ������� ����
void displayMenu() {
    std::cout << "\n����:\n"
        << "1. ������ �����\n"
        << "2. ����������� �� ������\n"
        << "3. ������� �����\n"
        << "4. �������� �����\n"
        << "5. ������ ������� ��������\n"
        << "6. ���������� ������� ������� �������� � ������������ ���������\n"
        << "7. ������ ���������� �� ���������� ���������\n"
        << "8. �����\n"
        << "��� ����: ";
}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    std::string filename;
    std::cout << "������ ��'� �����: ";
    std::cin >> filename;

    int choice;
    do {
        displayMenu();
        std::cin >> choice;
        switch (choice) {
        case 1: {
            Record record;
            std::cout << "������ ��� (��'� ���������, ��������, ����� ��������, ����, ���������): ";
            std::cin.ignore(); // ��� ���������� ���������� ����
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
            std::cout << "������ ������ ������ ��� �����: ";
            std::cin >> index;
            Record record;
            std::cout << "������ ��� ��� (��'� ���������, ��������, ����� ��������, ����, ���������): ";
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
            std::cout << "������ ������ ������ ��� ���������: ";
            std::cin >> index;
            deleteRecordFromFile(filename, index);
            break;
        }
        case 5: {
            double income, minUtility;
            std::cout << "������ �������� � �������� ���������: ";
            std::cin >> income >> minUtility;
            findAffordableProducts(filename, income, minUtility);
            break;
        }
        case 6:
            calculateMaxUtilityCost(filename);
            break;
        case 7: {
            double price, utility;
            std::cout << "������ ���� � ����� ���������: ";
            std::cin >> price >> utility;
            findConsumersBelowPrice(filename, price, utility);
            break;
        }
        case 8:
            std::cout << "�����.\n";
            break;
        default:
            std::cerr << "����������� ����. ��������� �� ���.\n";
        }
    } while (choice != 8);

    return 0;
}
