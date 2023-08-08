#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <pqxx/pqxx>


class DatabaseClients final {
public:
	enum class e_change {
		exit,
		name,
		surname,
		email
	};

	class Client final {
	public:
		explicit Client(const int id, const std::string& name, const std::string& surname, const std::string& email, const std::vector<std::string>& phone) :
			m_id(id), m_name(name), m_surname(surname), m_email(email), m_phone(phone) {}
		~Client() {}

		int get_id() const { return m_id; }
		std::string get_name() const { return m_name; }
		std::string get_surname() const { return m_surname; }
		std::string get_email() const { return m_email; }
		std::vector<std::string> phone() const { return m_phone; }

	private:
		int m_id;
		std::string m_name;
		std::string m_surname;
		std::string m_email;
		std::vector<std::string> m_phone;
	};

	DatabaseClients(const std::string& username, const std::string& db_name, const std::string& password);
	~DatabaseClients() {};

	DatabaseClients(const DatabaseClients& other) = delete;
	DatabaseClients(DatabaseClients&& other) noexcept = delete;
	DatabaseClients& operator=(const DatabaseClients& other) = delete;
	DatabaseClients& operator=(DatabaseClients&& other) noexcept = delete;

	void make_DB() const;
	std::vector<DatabaseClients::Client> get_DB() const;
	void addClient(const std::string& name, const std::string& surname, const std::string& email, const std::vector<std::string>& phone) const;
	void deleteClient(const int id_client) const;
	void addClientPhone(const int id_client, const std::vector<std::string>& phone) const;
	void deleteClientPhone(const int id_client, const int id_phone) const;
	void changeClient(const int id_client, const std::string& new_data, const e_change e_ch) const;
	std::vector<DatabaseClients::Client> findClient(const std::string& search_str, const e_change e_ch) const;

	int size() const;

	std::string get_name(const int id_client) const;
	std::string get_surname(const int id_client) const;
	std::string get_email(const int id_client) const;
	std::map<int, std::string> get_phone(const int id_client) const;

private:
	std::string m_username;
	std::string m_db_name;
	std::string m_password;
	std::unique_ptr<pqxx::connection> m_c;

	void exec(std::string str) const;
	void exec_prepared_add_Client(const std::string& name, const std::string& surname, const std::string& email) const;
	void exec_prepared_add_Phone(const std::string& phone) const;
	void exec_prepared_add_ClientPhone(const std::string& id_client, const std::string& id_phone) const;

	int get_number_of_Clients() const;
	int get_id_Client(const std::string& email) const;
	int get_id_Phone(const std::string& phone) const;
};
