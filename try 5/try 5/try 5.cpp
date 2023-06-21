#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <unordered_map>
#include <random>
#include <algorithm>
#include <fstream> // Для работы с файлами
#include <sstream> // Для работы со строками


class Product
{
protected:
    std::string COMPANY;
    std::string TITLE;
    std::vector<double> PRICE;
    double Max_Procent_Discount;

public:
    Product(std::string company, std::string title, double price, double max_procent_discount)
        : COMPANY(company), TITLE(title), Max_Procent_Discount(max_procent_discount)
    {
        PRICE.push_back(price);
    }

    double get_price(size_t index) const
    {
        if (index >= PRICE.size())
        {
            throw std::out_of_range("Invalid price index");
        }
        return PRICE[index];
    }

    size_t get_price_size() const
    {
        return PRICE.size();
    }

    double get_max_discount() const
    {
        return Max_Procent_Discount;
    }

    const std::string& getCompany() const
    {
        return COMPANY;
    }

    const std::string& getTitle() const
    {
        return TITLE;
    }

    void setCompany(const std::string& company)
    {
        COMPANY = company;
    }

    void setTitle(const std::string& title)
    {
        TITLE = title;
    }
};

class HouseholdAppliances : public Product
{
public:
    HouseholdAppliances(std::string company, std::string title, double price, double max_procent_discount)
        : Product(company, title, price, max_procent_discount) {}
};

class Hoover : public HouseholdAppliances
{
public:
    Hoover(std::string company, std::string title, double price, double max_procent_discount)
        : HouseholdAppliances(company, title, price, max_procent_discount) {}
};

class Camera : public Product
{
public:
    Camera(std::string company, std::string title, double price, double max_procent_discount)
        : Product(company, title, price, max_procent_discount) {}
};

class DSLRCamera : public Camera
{
public:
    DSLRCamera(std::string company, std::string title, double price, double max_procent_discount)
        : Camera(company, title, price, max_procent_discount) {}
};

class Notebook : public Product
{
private:
    double SIZE_DIAGONAL;
    double WEIGHT;
    int CORE;
    double MEMORY;

public:
    Notebook(std::string company, std::string title, double price, double max_procent_discount,
        double size_diagonal, double weight, int core, double memory)
        : Product(company, title, price, max_procent_discount),
        SIZE_DIAGONAL(size_diagonal), WEIGHT(weight), CORE(core), MEMORY(memory) {}
};

class User
{
public:
    int id;
    std::string full_name;
    double account_balance;
    double discount = 0.0;
    std::vector<Product> purchased_products;

    User() = default;

    User(int id, std::string full_name, double initial_balance)
        : id(id), full_name(std::move(full_name)), account_balance(initial_balance) {}

    void add_balance()
    {
        double amount;
        std::cout << "Enter the amount you wish to add to the balance: ";

        // Чтение ввода пользователя
        while (!(std::cin >> amount))
        {
            // Если ввод некорректный, очищаем ошибки и буфер ввода
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "An incorrect value has been entered. Try again: ";
        }

        if (amount < 0)
        {
            std::cout << "The amount cannot be negative. Please try again.\n";
            return;
        }

        account_balance += amount; // используем account_balance вместо AMOUNT_OF_MONEY
        std::cout << "Your balance has been successfully topped up. Current balance: " << account_balance << "\n";

    }

    virtual double individual_discount() const
    {
        return 0.0;
    }

    virtual bool purchase_product(Product& product, size_t price_index)
    {
        if (price_index >= product.get_price_size())
        {
            std::cout << "Invalid price index.\n";
            return false;
        }

        double max_discounted_price = product.get_price(price_index) * (1.0 - product.get_max_discount());

        double user_discount = individual_discount();

        double discounted_price = product.get_price(price_index) * (1.0 - std::max(user_discount, product.get_max_discount()));

        if (account_balance >= discounted_price)
        {
            account_balance -= discounted_price;
            purchased_products.push_back(product);
            return true; // purchase was successful
        }
        else
        {
            std::cout << "Insufficient funds.\n";
            return false; // insufficient funds
        }
    }
};

class RegularCustomer : public User
{
private:
    double total_purchase_cost;

public:
    RegularCustomer(std::string full_name, double initial_balance)
        : User(0, full_name, initial_balance), total_purchase_cost(0.0) {}

    double individual_discount() const override
    {
        return std::min(total_purchase_cost / 1000.0, 0.15);
    }

    bool purchase_product(Product& product, size_t price_index) override
    {
        bool purchase_successful = User::purchase_product(product, price_index);
        if (purchase_successful)
        {
            total_purchase_cost += product.get_price(price_index);
        }
        return purchase_successful;
    }
};



static void view_products(int user_id);

static void purchase_product(int user_id);



std::vector<Product> products; // Хранение товаров
std::unordered_map<int, User> users;

int generate_id()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<int> distr(1, 1000000);
    int id;
    do
    {
        id = distr(gen);
    } while (users.find(id) != users.end()); // Проверка на уникальность ID
    return id;
}

void save_users_to_file(const std::unordered_map<int, User>& users)
{
    //std::ofstream file("users.txt", std::ios::app); // Открытие файла в режиме добавления

    //if (!file)
    //{
    //    std::cerr << "Failed to open users.txt for writing.\n";
    //    return;
    //}
    
    std::ofstream file("users.txt"); // Открытие файла в режиме перезаписи

    if (!file)
    {
        std::cerr << "Failed to open users.txt for writing.\n";
        return;
    }



    for (const auto& pair : users)
    {
        const User& user = pair.second;
        file << user.id << '\n'; // Сохранение ID пользователя
        file << user.full_name << '\n';
        file << user.account_balance << '\n';
        file << user.discount << '\n';
        file << user.purchased_products.size() << '\n';
        for (const auto& product : user.purchased_products)
        {
            file << product.getCompany() << '\n';
            file << product.getTitle() << '\n';
            file << product.get_price(0) << '\n';
        }
    }

    file.close();
}

void load_users_from_file(std::unordered_map<int, User>& users)
{
    std::ifstream file("users.txt");
    if (!file)
    {
        // Если файл не существует, создаем его
        std::ofstream createFile("users.txt");
        createFile.close();

        // Проверяем, успешно ли создан файл
        if (!createFile)
        {
            std::cerr << "Failed to create users.txt file.\n";
            return;
        }

        // Открываем файл после его создания
        file.open("users.txt");
        if (!file)
        {
            std::cerr << "Failed to open users.txt for reading.\n";
            return;
        }
    }

    users.clear();

    std::string line;
    while (std::getline(file, line))
    {
        int user_id; // Добавлено: переменная для хранения ID пользователя
        file >> user_id;
        file.ignore(); // Игнорирование символа новой строки

        std::string full_name = line;
        double account_balance, discount;
        file >> account_balance >> discount;
        file.ignore(); // Игнорирование символа новой строки

        User user(user_id, full_name, account_balance); // Использование ID при создании пользователя
        user.discount = discount;

        int num_purchased_products;
        file >> num_purchased_products;
        file.ignore(); // Игнорирование символа новой строки

        for (int i = 0; i < num_purchased_products; ++i)
        {
            std::string company, title;
            double price;
            std::getline(file, company);
            std::getline(file, title);
            file >> price;
            file.ignore(); // Игнорирование символа новой строки

            Product product(company, title, price, 0.0);
            user.purchased_products.push_back(product);
        }

        users[user_id] = user;
    }

    file.close();
}

static void user_menu(int user_id)
{

    User& user = users[user_id];
    while (true)
    {
         
        std::cout << "-------------Hypermarket------------" << std::endl;
        std::cout << "1. View products" << std::endl;
        std::cout << "2. Purchase product" << std::endl;
        std::cout << "3. Top up your account" << std::endl;
        std::cout << "4. Exit" << std::endl;
        std::cout << "Enter your choice: " << std::endl;

        int choice;
        std::cin >> choice;
        std::cin.ignore();

        switch (choice)
        {
        case 1:
            view_products(user_id);
            break;
        case 2:
            purchase_product(user_id);
            break;
        case 3:
            users[user_id].add_balance();
            break;
        case 4:
            save_users_to_file(users);
            return; // выход в главное меню
        default:
            std::cout << "Invalid choice.\n";
            break;
        }
    }
}

void sign_up(std::unordered_map<int, User>& users)
{
    std::string full_name;
    double initial_balance;

    std::cout << "Enter your full name: ";
    std::getline(std::cin, full_name);

    std::cout << "Enter the initial amount of money: ";
    std::cin >> initial_balance;
    std::cin.ignore();

    int user_id = generate_id();
    User new_user(user_id, full_name, initial_balance);
    users[user_id] = new_user;

    std::cout << "You have successfully signed up! Your user ID is " << user_id << "\n";

    save_users_to_file(users); // Сохранение пользователей в файл
}

void sign_in()
{


    int user_id;
    std::cout << "Enter your user ID: ";
    std::cin >> user_id;
    std::cin.ignore();

    auto it = users.find(user_id);
    if (it == users.end())
    {
        std::cout << "User not found.\n";
    }
    else
    {
        std::cout << "Welcome back, " << it->second.full_name << "!\n";
        int user_id = it->second.id; // Сохранение ID пользователя
        user_menu(user_id);
    }
}

void purchase_product(int user_id)
{
    std::cout << "Enter the title of the product you want to purchase: ";
    std::string product_title;
    std::getline(std::cin, product_title);

    Product* product = nullptr;

    // Найти товар по введенному названию
    for (auto& p : products) {
        if (p.getTitle() == product_title) {
            product = &p;
            break;
        }
    }

    if (product == nullptr) {
        std::cout << "Product not found.\n";
        return;
    }

    User* user = nullptr;

    // Найти пользователя по user_id
    for (auto& u : users) {
        if (u.second.id == user_id) {
            user = &(u.second);
            break;
        }
    }

    if (user == nullptr) {
        std::cout << "User not found.\n";
        return;
    }

    // Проверить, достаточно ли у пользователя денег на покупку
    if (user->account_balance < product->get_price(0)) {
        std::cout << "Insufficient funds.\n";
        return;
    }

    // Выполнить покупку
    user->account_balance -= product->get_price(0);
    user->purchased_products.push_back(*product);

    std::cout << "Purchase successful!\n";

    save_users_to_file(users); // Сохранение данных пользователей в файл
}

void view_products(int user_id)
{
    auto it = users.find(user_id);
    if (it == users.end())
    {
        std::cout << "User not found.\n";
        return;
    }

    User& user = it->second;

    std::cout << "User: " << user.full_name << std::endl;
    std::cout << "Discount: " << user.discount * 100 << "%" << std::endl;
    std::cout << "Purchased products:" << std::endl;
    if (user.purchased_products.empty())
    {
        std::cout << "No products purchased." << std::endl;
    }
    else
    {
        for (const auto& product : user.purchased_products)
        {
            std::cout << "Company: " << product.getCompany() << std::endl;
            std::cout << "Title: " << product.getTitle() << std::endl;
            std::cout << "Price: " << product.get_price(0) << std::endl;  // Предполагаем, что у товара всегда есть хотя бы одна цена
            std::cout << "-------------------------" << std::endl;
        }
    }
}

void main_menu()
{
    load_users_from_file(users); // Загрузка пользователей из файла перед началом работы программы

    while (true)
    {
        std::cout << "1. Sign up" << std::endl;
        std::cout << "2. Sign in" << std::endl;
        std::cout << "3. Exit" << std::endl;
        std::cout << "Enter your choice: " << std::endl;

        int choice;
        std::cin >> choice;
        std::cin.ignore();

        switch (choice)
        {
        case 1:
            sign_up(users);
            break;
        case 2:
            sign_in();
            break;
        case 3:
            save_users_to_file(users);
            return; // выход из программы
        default:
            std::cout << "Invalid choice.\n";
            break;
        }
    }
}




int main()
{

    products.push_back(Product("Company 1", "Product 1", 10.0, 1));
    products.push_back(Product("Company 2", "Product 2", 20.0, 0.2));
    products.push_back(Product("Company 3", "Product 3", 30.0, 0.3));


    load_users_from_file(users);
    main_menu();
    return 0;
}
