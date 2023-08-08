#include "main.h"


int main(){
	SetConsoleCP(1251);
	setlocale(LC_ALL, "ru_RU.UTF-8");
	print_title();
	
	std::string username, db_name, password;
	std::unique_ptr<DatabaseClients> db_clnt;

	std::cout << "Введите имя пользователя(без пробелов): ";
	std::cin >> username;
	std::cout << "Введите название базы данных(без пробелов): ";
	std::cin >> db_name;
	std::cout << "Введите пароль(без пробелов): ";
	std::cin >> password;
	std::cout << std::endl;

	try {
		db_clnt = std::make_unique<DatabaseClients>(username, db_name, password);
		std::cout << "Подключение успешно установлено!" << std::endl;

		db_clnt->make_DB();
		wait_user();
	}
	catch (std::exception& ex) {
		std::cout << ex.what() << std::endl;
		wait_user();
	}

	main_menu(db_clnt);

	return 0;
}

void main_menu(const std::unique_ptr<DatabaseClients>& db_clnt) {
	while (true) {
		print_title();
		std::cout << "\tГлавное меню\n" << std::endl;
		std::cout << "Количество записей в базе: " << db_clnt->size() << std::endl << std::endl;

		std::cout << "Выбирите действие:" << std::endl;
		std::cout <<
			"1 - вывести базу данных\n"
			"2 - добавить клиента\n"
			"3 - удалить клиента\n"
			"4 - добавить телефон для существующего клиента\n"
			"5 - удалить телефон для существующего клиента\n"
			"6 - изменить данные о клиенте\n"
			"7 - найти клиента по его данным\n"
			"0 - выход\n" << std::endl;

		int select = 0;
		std::cin >> select;
		while (select < 0 || select > 7) {
			std::cout << "Неккоректный ввод! Введите еще раз:" << std::endl;
			std::cin >> select;
		}

		if (select == static_cast<int>(e_main_menu::exit)) {
			return;
		}
		else if (select == static_cast<int>(e_main_menu::printDatabase)) {
			print_DB(db_clnt);
		}
		else if (select == static_cast<int>(e_main_menu::addClient)) {
			menu_add_client(db_clnt);
		}
		else if (select == static_cast<int>(e_main_menu::deleteClient)) {
			menu_delete_client(db_clnt);
		}
		else if (select == static_cast<int>(e_main_menu::addPhone)) {
			menu_add_phone(db_clnt);
		}
		else if (select == static_cast<int>(e_main_menu::deletePhone)) {
			menu_delete_phone(db_clnt);
		}
		else if (select == static_cast<int>(e_main_menu::changeClient)) {
			menu_change_client(db_clnt);
		}
		else if (select == static_cast<int>(e_main_menu::findClient)) {
			menu_find_client(db_clnt);
		}
	}
}

void print_DB(const std::unique_ptr<DatabaseClients>& db_clnt) {
	print_title();

	std::vector<DatabaseClients::Client> clients;

	try {
		clients = std::move(db_clnt->get_DB());
	}
	catch (std::exception& ex) {
		std::cout << ex.what() << std::endl;
		wait_user();
	}
	
	std::cout << "\nID  Имя             Фамилия             email                         Телефон" << std::endl;
	std::cout << "---------------------------------------------------------------------------------" << std::endl;
	int i = 0;
	for (const auto& it_c : clients) {
		set_cursor(0, 7 + i);
		std::cout << it_c.get_id();
		set_cursor(4, 7 + i);
		std::cout << it_c.get_name();
		set_cursor(20, 7 + i);
		std::cout << it_c.get_surname();
		set_cursor(40, 7 + i);
		std::cout << it_c.get_email();

		for (const auto& it_p : it_c.phone()) {
			set_cursor(70, 7 + i);
			std::cout << it_p;
			i++;
		}
		if (it_c.phone().empty()) {
			i += 2;
		}
		else {
			i++;
		}
	}
	std::cout << "\n\n---------------------------------------------------------------------------------" << std::endl << std::endl;

	wait_user();
}

void menu_add_client(const std::unique_ptr<DatabaseClients>& db_clnt) {
	print_title();
	std::cout << "\tДобавить клиента\n" << std::endl;

	std::string name, surname, email, number;
	int enter_phone = 0;
	std::vector<std::string> phone;

	std::cout << "Имя: ";
	std::cin >> name;
	std::cout << "Фамилия: ";
	std::cin >> surname;
	std::cout << "email: ";
	std::cin >> email;

	std::cout << "Внести телефон(1 - да, 0 - нет)?: ";
	std::cin >> enter_phone;
	while (enter_phone < 0 || enter_phone > 1) {
		std::cout << "Неккоректный ввод! Введите еще раз: ";
		std::cin >> enter_phone;
	}

	if (enter_phone) {
		while (enter_phone) {
			std::cout << "Номер: ";
			std::cin >> number;
			phone.emplace_back(number);
			std::cout << "Eще номер(1 - да, 0 - нет)?: ";
			std::cin >> enter_phone;
			while (enter_phone < 0 || enter_phone > 1) {
				std::cout << "Неккоректный ввод! Введите еще раз: ";
				std::cin >> enter_phone;
			}
		}
	}

	name = cp1251_to_utf8(name.c_str());
	surname = cp1251_to_utf8(surname.c_str());
	email = cp1251_to_utf8(email.c_str());

	for (auto& it : phone) {
		it = cp1251_to_utf8(it.c_str());
	}

	try {
		db_clnt->addClient(name, surname, email, phone);
		std::cout << "\nКлиент успешно добавлен!" << std::endl;
		wait_user();
	}
	catch (std::exception& ex) {
		std::cout << ex.what() << std::endl;
		wait_user();
	}
}

void menu_delete_client(const std::unique_ptr<DatabaseClients>& db_clnt) {
	print_title();
	std::cout << "\tУдалить клиента\n" << std::endl;

	int id_client = 0;

	std::cout << "ID клиента: ";
	std::cin >> id_client;
	while (id_client < 1) {
		std::cout << "Неккоректный ввод! Введите еще раз: ";
		std::cin >> id_client;
	}

	try {
		db_clnt->deleteClient(id_client);
		std::cout << "\nКлиент успешно удален!" << std::endl;
		wait_user();
	}
	catch (std::exception& ex) {
		std::cout << ex.what() << std::endl;
		wait_user();
	}
}

void menu_add_phone(const std::unique_ptr<DatabaseClients>& db_clnt) {
	print_title();
	std::cout << "\tДобавить телефон\n" << std::endl;

	int id_client = 0;
	std::string number;
	int enter_phone = 1;
	std::vector<std::string> phone;

	std::cout << "ID клиента: ";
	std::cin >> id_client;
	while (id_client < 1) {
		std::cout << "Неккоректный ввод! Введите еще раз: ";
		std::cin >> id_client;
	}

	while (enter_phone) {
		std::cout << "Телефоный номер: ";
		std::cin >> number;
		phone.emplace_back(number);
		std::cout << "Eще номер(1 - да, 0 - нет)?: ";
		std::cin >> enter_phone;
		while (enter_phone < 0 || enter_phone > 1) {
			std::cout << "Неккоректный ввод! Введите еще раз: ";
			std::cin >> enter_phone;
		}
	}

	for (auto& it : phone) {
		it = cp1251_to_utf8(it.c_str());
	}

	try {
		db_clnt->addClientPhone(id_client, phone);
		std::cout << "\nТелефон(ы) успешно добавлены!" << std::endl;
		wait_user();
	}
	catch (std::exception& ex) {
		std::cout << ex.what() << std::endl;
		wait_user();
	}
}

void menu_delete_phone(const std::unique_ptr<DatabaseClients>& db_clnt) {
	print_title();
	std::cout << "\tУдалить телефон\n" << std::endl;

	int id_client = 0;
	int id_phone = 0;
	std::map<int, std::string> phones;

	std::cout << "ID клиента: ";
	std::cin >> id_client;
	while (id_client < 1) {
		std::cout << "Неккоректный ввод! Введите еще раз: ";
		std::cin >> id_client;
	}

	try {
		std::cout << "\nID: " << id_client << std::endl;
		std::cout << "Имя: " << db_clnt->get_name(id_client) << std::endl;
		std::cout << "Фамилия: " << db_clnt->get_surname(id_client) << std::endl;
		std::cout << "email: " << db_clnt->get_email(id_client) << std::endl << std::endl;

		phones = std::move(db_clnt->get_phone(id_client));

		std::cout << "\nТелефоны:" << std::endl;
		for (const auto& it : phones) {
			std::cout << "ID: " << it.first << std::endl;
			std::cout << "Номер: " << it.second << std::endl << std::endl;
		}

		std::cout << "Введите ID телефонного номера, который нужно удалить(0 - отмена): ";
		std::cin >> id_phone;
		while (id_client < 0) {
			std::cout << "Неккоректный ввод! Введите еще раз: ";
			std::cin >> id_client;
		}

		if (id_phone == 0) {
			return;
		}
		else {
			db_clnt->deleteClientPhone(id_client, id_phone);
			std::cout << "\nТелефон успешно удален!" << std::endl;
			wait_user();
		}
	}
	catch (std::exception& ex) {
		std::cout << ex.what() << std::endl;
		wait_user();
	}
}

void menu_change_client(const std::unique_ptr<DatabaseClients>& db_clnt) {
	print_title();
	std::cout << "\tИзменить данные о клиенте\n" << std::endl;

	int id_client = 0;
	std::string new_data;

	std::cout << "ID клиента: ";
	std::cin >> id_client;
	while (id_client < 1) {
		std::cout << "Неккоректный ввод! Введите еще раз: ";
		std::cin >> id_client;
	}

	try {
		std::cout << "\nID: " << id_client << std::endl;
		std::cout << "Имя: " << db_clnt->get_name(id_client) << std::endl;
		std::cout << "Фамилия: " << db_clnt->get_surname(id_client) << std::endl;
		std::cout << "email: " << db_clnt->get_email(id_client) << std::endl << std::endl;
	}
	catch (std::exception& ex) {
		std::cout << ex.what() << std::endl;
		wait_user();
		return;
	}

	std::cout << "Изменить:" << std::endl;
	std::cout <<
		"1 - Имя\n"
		"2 - Фамилия\n"
		"3 - email\n"
		"0 - выход\n" << std::endl;

	int select = 0;
	std::cin >> select;
	while (select < 0 || select > 3) {
		std::cout << "Неккоректный ввод! Введите еще раз:" << std::endl;
		std::cin >> select;
	}

	try {
		if (select == static_cast<int>(DatabaseClients::e_change::exit)) {
			return;
		}
		else if (select == static_cast<int>(DatabaseClients::e_change::name)) {
			std::cout << "Введите новое имя: ";
			std::cin >> new_data;
			new_data = cp1251_to_utf8(new_data.c_str());
			db_clnt->changeClient(id_client, new_data, DatabaseClients::e_change::name);
		}
		else if (select == static_cast<int>(DatabaseClients::e_change::surname)) {
			std::cout << "Введите новую фамилию: ";
			std::cin >> new_data;
			new_data = cp1251_to_utf8(new_data.c_str());
			db_clnt->changeClient(id_client, new_data, DatabaseClients::e_change::surname);
		}
		else if (select == static_cast<int>(DatabaseClients::e_change::email)) {
			std::cout << "Введите новый email: ";
			std::cin >> new_data;
			new_data = cp1251_to_utf8(new_data.c_str());
			db_clnt->changeClient(id_client, new_data, DatabaseClients::e_change::email);
		}

		std::cout << "\nДанные успешно изменены!" << std::endl;
		wait_user();
	}
	catch (std::exception& ex) {
		std::cout << ex.what() << std::endl;
		wait_user();
	}
}

void menu_find_client(const std::unique_ptr<DatabaseClients>& db_clnt) {
	print_title();
	std::cout << "\tПоиск\n" << std::endl;

	std::string search_str;
	int i = 0;

	std::cout << "Поиск по:" << std::endl;
	std::cout <<
		"1 - Имя\n"
		"2 - Фамилия\n"
		"3 - email\n"
		"0 - выход\n" << std::endl;

	int select = 0;
	std::cin >> select;
	while (select < 0 || select > 3) {
		std::cout << "Неккоректный ввод! Введите еще раз: ";
		i++;
		std::cin >> select;
	}

	std::cout << "Что найти?: ";
	std::cin >> search_str;
	std::cout << std::endl;
	search_str = cp1251_to_utf8(search_str.c_str());

	try {
		std::unique_ptr<std::vector<DatabaseClients::Client>> clients;

		if (select == static_cast<int>(DatabaseClients::e_change::exit)) {
			return;
		}
		else if (select == static_cast<int>(DatabaseClients::e_change::name)) {
			clients = std::make_unique<std::vector<DatabaseClients::Client>>(db_clnt->findClient(search_str, DatabaseClients::e_change::name));
		}
		else if (select == static_cast<int>(DatabaseClients::e_change::surname)) {
			clients = std::make_unique<std::vector<DatabaseClients::Client>>(db_clnt->findClient(search_str, DatabaseClients::e_change::surname));
		}
		else if (select == static_cast<int>(DatabaseClients::e_change::email)) {
			clients = std::make_unique<std::vector<DatabaseClients::Client>>(db_clnt->findClient(search_str, DatabaseClients::e_change::email));
		}
		std::cout << "Найдено:" << std::endl;
		std::cout << "\nID  Имя             Фамилия             email                         Телефон" << std::endl;
		std::cout << "---------------------------------------------------------------------------------" << std::endl;
		for (const auto& it_c : *clients) {
			set_cursor(0, 19 + i);
			std::cout << it_c.get_id();
			set_cursor(4, 19 + i);
			std::cout << it_c.get_name();
			set_cursor(20, 19 + i);
			std::cout << it_c.get_surname();
			set_cursor(40, 19 + i);
			std::cout << it_c.get_email();

			for (const auto& it_p : it_c.phone()) {
				set_cursor(70, 19 + i);
				std::cout << it_p;
				i++;
			}
			if (it_c.phone().empty()) {
				i += 2;
			}
			else {
				i++;
			}
		}
		std::cout << "\n\n---------------------------------------------------------------------------------" << std::endl << std::endl;

		wait_user();
	}
	catch (std::exception& ex) {
		std::cout << ex.what() << std::endl;
		wait_user();
	}
}

std::string cp1251_to_utf8(const char* str) {
	std::string res;
	int result_u, result_c;
	result_u = MultiByteToWideChar(1251, 0, str, -1, 0, 0);
	if (!result_u) { return ""; }
	wchar_t* ures = new wchar_t[result_u];
	if (!MultiByteToWideChar(1251, 0, str, -1, ures, result_u)) {
		delete[] ures;
		return "";
	}
	result_c = WideCharToMultiByte(65001, 0, ures, -1, 0, 0, 0, 0);
	if (!result_c) {
		delete[] ures;
		return "";
	}
	char* cres = new char[result_c];
	if (!WideCharToMultiByte(65001, 0, ures, -1, cres, result_c, 0, 0)) {
		delete[] cres;
		return "";
	}
	delete[] ures;
	res.append(cres);
	delete[] cres;
	return res;
}

void print_title() {
	system("cls");
	std::cout << "\tРабота с PostgreSQL из C++\n\n" << std::endl;
}

void wait_user() {
	std::cout << "\nНажмите любую клавишу для продолжения..." << std::endl;
	system("pause > nul");
}

void set_cursor(const int x, const int y) {
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { static_cast<short>(x), static_cast<short>(y) });
}
