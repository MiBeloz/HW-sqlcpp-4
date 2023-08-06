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
	}
	catch (std::exception& ex) {
		std::cout << ex.what() << std::endl;
		std::cout << "Нажмите любую клавишу для продолжения..." << std::endl;
		system("pause > nul");
		return 1;
	}

	db_clnt->make_DB();

	{
		std::string name, surname, email, number;
		int enter_phone = 0;
		std::vector<std::string> phone;

		std::cout << "Имя: ";
		std::cin >> name;
		std::cout << "Фамилия: ";
		std::cin >> surname;
		std::cout << "email: ";
		std::cin >> email;

		std::cout << "Внести телефон?" << std::endl;
		std::cin >> enter_phone;

		if (enter_phone) {
			while (enter_phone) {
				std::cout << "Номер: ";
				std::cin >> number;
				phone.emplace_back(number);
				std::cout << "Eще номер?" << std::endl;
				std::cin >> enter_phone;
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
		}
		catch (std::exception& ex) {
			std::cout << ex.what() << std::endl;
			std::cout << "Нажмите любую клавишу для продолжения..." << std::endl;
			system("pause > nul");
			return 1;
		}
	}
	


		//pqxx::work tx(c);
		//for (const auto& [name, surname, email] : tx.query<std::string, std::string, std::string>("select name, surname, email from client")) {
		//	std::cout << "Имя: " << name << std::endl;
		//	std::cout << "Фамилия: " << surname << std::endl;
		//	std::cout << "email: " << email << std::endl;
		//	std::cout << std::endl;
		//}

		////tx.exec("insert into client values(3, 'Валентина', 'Белозерова', 'valushka@mail.ru')");
		//tx.commit();

		////pqxx::work tx2(c);
		////tx2.exec("update client set email='belozerova.vv@omsk.gazprom-neft.ru' where email='valushka@mail.ru'");
		////tx2.exec("update client set email='" + ggg + "' where email='ggg'");
		////tx2.commit();
	
	system("pause > nul");
	return 0;
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
	std::cout << "\tРабота с PostgreSQL из C++\n\n" << std::endl;
}















//create table if not exists client(
//	id serial primary key,
//	name text not null,
//	surname text not null,
//	email text unique not null
//);
//
//create table if not exists phone(
//	id serial primary key,
//	number text unique not null
//);
//
//create table if not exists clientphone(
//	client_id integer references client(id),
//	phone_id integer references phone(id),
//	constraint pk_clientphone primary key(client_id, phone_id)
//);
//
//
//insert into client(name, surname, email)
//values('михаил', 'белозеров', 'mihabeloz@gmail.com'), ('татьяна', 'белозерова', 't.kytuzova@gmail.com');
//
//insert into phone(number)
//values('89083179589'), ('89059404572'), ('89609908802');
//
//insert into clientphone(client_id, phone_id)
//values(1, 1), (1, 2), (2, 3);
