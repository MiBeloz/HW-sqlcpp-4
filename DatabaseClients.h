#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <Windows.h>
#include <pqxx/pqxx>


class DatabaseClients final {
public:
	DatabaseClients(const std::string& username, const std::string& db_name, const std::string& password);
	~DatabaseClients() {};

	DatabaseClients(const DatabaseClients& other) = delete;
	DatabaseClients(DatabaseClients&& other) noexcept = delete;
	DatabaseClients& operator=(const DatabaseClients& other) = delete;
	DatabaseClients& operator=(DatabaseClients&& other) noexcept = delete;

	void make_DB() const;
	void print_DB() const;
	void addClient(const std::string& name, const std::string& surname, const std::string& email, const std::vector<std::string>& phone) const;
	void addClientPhone(const int id_client, const std::vector<std::string>& phone) const;
	void changeClient(const int id_client, const std::string& name, const std::string& surname, const std::string& email, const std::vector<std::string>& phone);
	//void deleteClientPhone();
	//void deleteClient();
	//void findClient();

	int size() const;

	std::string get_name(const int id_client);
	std::string get_surname(const int id_client);
	std::string get_email(const int id_client);
	std::vector<std::string> get_phone(const int id_client);

private:
	std::string m_username;
	std::string m_db_name;
	std::string m_password;
	std::unique_ptr<pqxx::connection> m_c;

	void set_cursor(const int x, const int y) const;

	void exec(std::string str) const;
	void exec_prepared_add_Client(const std::string& name, const std::string& surname, const std::string& email) const;
	void exec_prepared_add_Phone(const std::string& phone) const;
	void exec_prepared_add_ClientPhone(const std::string& id_client, const std::string& id_phone) const;

	int get_number_of_Clients() const;
	int get_id_Client(const std::string& email) const;
	int get_id_Phone(const std::string& phone) const;
};
