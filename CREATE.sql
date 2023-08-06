CREATE TABLE IF NOT EXISTS Client (
	id SERIAL PRIMARY KEY,
	name text NOT NULL,
	surname text NOT NULL,
	email text UNIQUE NOT null
);

CREATE TABLE IF NOT EXISTS Phone (
	id SERIAL PRIMARY KEY,
	number text UNIQUE NOT NULL
);

CREATE TABLE IF NOT EXISTS ClientPhone (
	client_id INTEGER REFERENCES Client(id),
	phone_id INTEGER REFERENCES Phone(id),
	CONSTRAINT pk_ClientPhone PRIMARY KEY (client_id, phone_id)
);


INSERT INTO Client(name, surname, email)
VALUES ('Михаил', 'Белозеров', 'mihabeloz@gmail.com'), ('Татьяна', 'Белозерова', 't.kytuzova@gmail.com');

INSERT INTO Phone(number)
VALUES ('89083179589'), ('89059404572'), ('89609908802');

INSERT INTO ClientPhone(client_id, phone_id)
VALUES (1, 1), (1, 2), (2, 3);