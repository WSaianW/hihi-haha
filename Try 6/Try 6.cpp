#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>

class Product {
protected:
    std::string company;
    std::string title;
    double price;
    double maxDiscount;

public:
    Product(const std::string& company, const std::string& title, double price, double maxDiscount)
        : company(company), title(title), price(price), maxDiscount(maxDiscount) {

        if (company.empty()) {
            throw std::invalid_argument("Company cannot be empty");
        }
        if (title.empty()) {
            throw std::invalid_argument("Title cannot be empty");
        }
        if (price < 0) {
            throw std::invalid_argument("Price cannot be negative");
        }
        if (maxDiscount < 0 || maxDiscount > 1) {
            throw std::invalid_argument("Invalid maximum discount");
        }
    }

    virtual ~Product() {}

    const std::string& getCompany() const {
        return company;
    }

    const std::string& getTitle() const {
        return title;
    }

    double getPrice() const {
        return price;
    }

    double getMaxDiscount() const {
        return maxDiscount;
    }

    virtual double calculateDiscount(double customerDiscount) const {
        double applicableDiscount = std::min(customerDiscount, maxDiscount);
        return price * (1 - applicableDiscount);
    }
};

class HouseholdAppliance : public Product {
public:
    HouseholdAppliance(const std::string& company, const std::string& title, double price, double maxDiscount)
        : Product(company, title, price, maxDiscount) {}
};

class VacuumCleaner : public HouseholdAppliance {
public:
    VacuumCleaner(const std::string& company, const std::string& title, double price, double maxDiscount)
        : HouseholdAppliance(company, title, price, maxDiscount) {}
};

class Camera : public Product {
public:
    Camera(const std::string& company, const std::string& title, double price, double maxDiscount)
        : Product(company, title, price, maxDiscount) {}
};

class DSLRCamera : public Camera {
public:
    DSLRCamera(const std::string& company, const std::string& title, double price, double maxDiscount)
        : Camera(company, title, price, maxDiscount) {}
};

class Laptop : public Product {
private:
    double screenSize;
    double weight;
    int processorCores;
    double memory;

public:
    Laptop(const std::string& company, const std::string& title, double price, double maxDiscount,
        double screenSize, double weight, int processorCores, double memory)
        : Product(company, title, price, maxDiscount),
        screenSize(screenSize), weight(weight), processorCores(processorCores), memory(memory) {

        if (screenSize < 0) {
            throw std::invalid_argument("Screen size cannot be negative");
        }

        if (weight < 0) {
            throw std::invalid_argument("Weight cannot be negative");
        }

        if (processorCores < 0) {
            throw std::invalid_argument("Processor cores cannot be negative");
        }
        if (memory < 0) {
            throw std::invalid_argument("Memory cannot be negative");
        }
    }
};

class Customer {
protected:
    double money;

public:
    Customer(double money) : money(money) {

        if (money < 0) {
            throw std::invalid_argument("Money cannot be negative");
        }
    }

    virtual ~Customer() {}

    double getMoney() const {
        return money;
    }

    virtual double calculateIndividualDiscount() const {
        return 0.0;
    }

    bool buyProduct(const Product& product, double customerDiscount) {
        double discountedPrice = product.calculateDiscount(customerDiscount);

        if (money >= discountedPrice) {
            money -= discountedPrice;
            return true; // Purchase successful
        }
        else {
            std::cout << "Insufficient funds." << std::endl;
            return false; // Insufficient funds
        }
    }
};

class RegularCustomer : public Customer {
private:
    std::string fullName;
    double totalPurchaseCost;

public:
    RegularCustomer(const std::string& fullName, double money)
        : Customer(money), fullName(fullName), totalPurchaseCost(0.0) {

        if (fullName.empty()) {
            throw std::invalid_argument("Full name cannot be empty");
        }
    }

    const std::string& getFullName() const {
        return fullName;
    }

    double getTotalPurchaseCost() const {
        return totalPurchaseCost;
    }

    double calculateIndividualDiscount() const override {
        double discount = totalPurchaseCost / 1000.0;
        return (discount <= 0.15) ? discount : 0.15;
    }

    bool buyProduct(const Product& product) {
        double customerDiscount = calculateIndividualDiscount();
        return Customer::buyProduct(product, customerDiscount);
    }

    void updateTotalPurchaseCost(double purchaseCost) {
        totalPurchaseCost += purchaseCost;
    }
};

int main() {
    try {
        std::vector<Product> products;

        HouseholdAppliance appliance1("Samsung", "Washing Machine", 500.0, 0.1);
        products.push_back(appliance1);

        VacuumCleaner vacuumCleaner1("Philips", "Vacuum Cleaner", 150.0, 0.2);
        products.push_back(vacuumCleaner1);

        Camera camera1("Canon", "Digital Camera", 300.0, 0.15);
        products.push_back(camera1);

        DSLRCamera dslrCamera1("Nikon", "DSLR Camera", 800.0, 0.25);
        products.push_back(dslrCamera1);

        Laptop laptop1("HP", "Laptop", 1000.0, 0.2, 15.6, 2.5, 4, 8.0);
        products.push_back(laptop1);

        RegularCustomer regularCustomer("John Doe", 2000.0);

        std::cout << "Regular Customer: " << regularCustomer.getFullName() << std::endl;
        std::cout << "Initial Balance: " << regularCustomer.getMoney() << std::endl;
        std::cout << "------------------------------" << std::endl;

        for (const auto& product : products) {
            std::cout << "Product: " << product.getCompany() << " - " << product.getTitle() << std::endl;
            if (regularCustomer.buyProduct(product)) {
                std::cout << "Purchase successful! Remaining Balance: " << regularCustomer.getMoney() << std::endl;
                regularCustomer.updateTotalPurchaseCost(product.getPrice());
            }
            std::cout << "------------------------------" << std::endl;
        }

        std::cout << "Total Purchase Cost: " << regularCustomer.getTotalPurchaseCost() << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "Exception occurred: " << e.what() << std::endl;
    }

    return 0;
}
