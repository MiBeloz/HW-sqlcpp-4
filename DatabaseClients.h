#pragma once

#include <string>
#include <vector>
#include <memory>
#include <pqxx/pqxx>


class DatabaseClients final {
public:
	DatabaseClients(std::string& username, std::string& db_name, std::string& password);
	~DatabaseClients() {};

	DatabaseClients(const DatabaseClients& other) = delete;
	DatabaseClients(DatabaseClients&& other) noexcept = delete;
	DatabaseClients& operator=(const DatabaseClients& other) = delete;
	DatabaseClients& operator=(DatabaseClients&& other) noexcept = delete;

	void make_DB();
	void addClient(std::string& name, std::string& surname, std::string& email, std::vector<std::string>& phone);
	//void addClientPhone();
	//void changeClient();
	//void deleteClientPhone();
	//void deleteClient();
	//void findClient();

private:
	std::string m_username;
	std::string m_db_name;
	std::string m_password;
	std::unique_ptr<pqxx::connection> m_c;
};
