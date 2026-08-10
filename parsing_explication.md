# Documentation — Parsing de la configuration du webserv

## Introduction

Cette partie du projet a pour rôle de transformer un fichier de configuration texte (`.conf`) en données utilisables par le serveur. Elle se déroule en deux temps, assurés par deux fichiers distincts :

1. **La vérification du chemin** : s'assurer que le fichier passé en argument est réellement utilisable avant même d'essayer de le lire.
2. **Le parsing du contenu** : lire ce fichier, comprendre sa syntaxe, et construire les structures de données qui décrivent chaque serveur et chaque route à gérer.

L'objectif de ce document est de permettre à n'importe qui de l'équipe de comprendre cette logique, de la maintenir, ou de l'étendre, sans avoir à tout redécouvrir seul.

---

## Partie 1 — Vérification du fichier de configuration

Avant de faire quoi que ce soit avec le fichier, on vérifie qu'il est exploitable. Cette étape est volontairement séparée du reste : elle ne s'occupe que du fichier en tant qu'objet du système (son nom, son existence, son type, ses droits, sa taille), jamais de ce qu'il contient. C'est une garde d'entrée : si elle échoue, on ne tente même pas de lire le contenu.

Cinq vérifications sont effectuées, toujours dans le même ordre, et la première qui échoue arrête immédiatement le processus :

```
void check(const std::string& path)
{
	checkExtension(path);
	checkExist(path);
	checkIsRegular(path);
	checkReadable(path);
	checkNotEmpty(path);
}
```

- **L'extension du fichier** est d'abord contrôlée, en vérifiant que le chemin se termine bien par `.conf`. C'est le contrôle le plus simple, il ne touche pas au disque, donc il n'y a aucun intérêt à faire les vérifications suivantes si celle-ci échoue déjà.
- **L'existence du fichier** est ensuite vérifiée. Si le fichier n'existe pas à l'emplacement indiqué, inutile d'aller plus loin.
- **Le type de fichier** est contrôlé pour s'assurer qu'il s'agit bien d'un fichier "normal", et non d'un dossier ou d'un autre type d'élément du système de fichiers qui porterait le même nom.
- **Les droits de lecture** sont vérifiés séparément, car un fichier peut exister et être du bon type tout en étant inaccessible en lecture pour l'utilisateur qui lance le programme.
- **Le contenu non vide** est vérifié en dernier : un fichier de configuration vide n'a de toute façon rien à parser, mais c'est une vérification "métier", propre à notre besoin, donc elle vient après toutes les vérifications plus génériques liées au système de fichiers.

Chacune de ces cinq étapes a sa propre erreur dédiée, avec un message explicite, ce qui permet d'afficher à l'utilisateur exactement ce qui ne va pas plutôt qu'un message générique. Une fois ces cinq vérifications passées avec succès, on a la certitude que le fichier peut être ouvert et lu sans mauvaise surprise, et le programme peut passer à l'étape suivante.

---

## Partie 2 — Parsing du contenu du fichier

Une fois le fichier validé, il faut en lire le contenu et le transformer en structures compréhensibles par le serveur. Cette étape se déroule elle-même en trois phases.

### Découpage du texte en unités logiques

Le fichier est d'abord lu ligne par ligne, et chaque ligne est découpée en petites unités appelées tokens : des mots-clés, des valeurs, ou des symboles de syntaxe isolés comme les accolades ouvrante et fermante ou le point-virgule.

```
static std::vector<std::string> tokenize(const std::string &filepath)
{
	std::ifstream file(filepath.c_str());
	if (!file.is_open())
		throw Config::ConfigException("Cannot open file: " + filepath);
	
	std::vector<std::string>	tokens;
	std::string 				line;

	while (std::getline(file, line))
	{
		//suppression des commentaires
		size_t	comment = line.find('#');
		if (comment != std::string::npos)
			line = line.substr(0, comment);

		std::istringstream	iss(line);
		std::string			word;
		while (iss >> word)
		{
			for (size_t i=0; i < word.size(); i++)
			{
				if (word[i] == '{' || word[i] == '}' || word[i] == ';')
				{
					if (i > 0)
						tokens.push_back(word.substr(0, i));
					tokens.push_back(std::string(1, word[i]));
					word = word.substr(i + 1);
					i = -1;
				}
			}
			if (!word.empty())
				tokens.push_back(word);
		}
	}
	return tokens;
}
```

Pendant cette étape, tout ce qui suit un symbole de commentaire sur une ligne est ignoré, ce qui permet d'annoter le fichier de configuration sans perturber le parsing. Le découpage gère aussi le cas où un symbole de syntaxe est collé directement à un mot, par exemple lorsqu'une valeur est immédiatement suivie d'un point-virgule sans espace : le mot et le symbole sont alors bien séparés en deux tokens distincts, pour que la suite du parsing n'ait jamais à se soucier de ce genre de détail de mise en forme.

Le résultat de cette phase est une longue liste plate de tokens, dans l'ordre où ils apparaissent dans le fichier, sans plus aucune notion de ligne ou de mise en page.

### Construction des blocs de configuration

Cette liste de tokens est ensuite parcourue séquentiellement à l'aide d'un curseur qui avance token par token. Ce parcours reconnaît une structure imbriquée à deux niveaux, qui reflète l'organisation attendue du fichier de configuration : chaque configuration contient un ou plusieurs blocs "serveur", et chaque bloc "serveur" contient à son tour un ou plusieurs blocs "location", qui décrivent le comportement du serveur pour une route particulière.

```
while (i < tokens.size())
{
	if (tokens[i] == "server")
	{
		i++;
		_servers.push_back(parse_server(tokens, i));
	}
	else
		throw Config::ConfigException("Expected 'server' block, got : " + tokens[i]);
}
```

Pour chaque bloc "serveur", les directives possibles définissent l'adresse d'écoute, le port, le nom du serveur, un dossier racine, une taille maximale de corps de requête, ainsi que des pages d'erreur personnalisées associées à des codes HTTP précis. Chaque directive est contrôlée individuellement : un port doit être un nombre dans la plage valide des ports réseau, une adresse doit être une adresse IP correctement formée ou le mot-clé désignant la machine locale, et la plupart des directives ne peuvent apparaître qu'une seule fois par bloc, sous peine d'erreur. Une fois le bloc entièrement lu, des vérifications finales s'assurent que les informations indispensables ont bien été fournies, en particulier qu'un port d'écoute a été précisé, qu'il est valide, et qu'au moins une route a été définie pour ce serveur. Si aucune adresse d'écoute n'a été précisée, une valeur par défaut est appliquée automatiquement, correspondant à une écoute sur toutes les interfaces réseau disponibles.

```
static ServerBlock parse_server(const std::vector<std::string> &tokens, size_t &i)
{
	ServerBlock server;

	bool listen_set = false;
	bool host_set = false;
	bool root_set = false;
	bool client_max_body_size_set = false;

	if (tokens[i++] != "{")
		throw Config::ConfigException("Expected '{' after server");
	
	std::cout << "DEBUG : start server parsing" << std::endl;

	while (i < tokens.size() && tokens[i] != "}")
	{
		std::string key = tokens[i++];

		std::cout << "DEBUG: key = " << key << std::endl;

		if (key == "listen")
		{
			expect_value(tokens, i, key);
			if (listen_set)
				throw Config::ConfigException("Duplicate 'listen' directive");
			if (!is_number(tokens[i]))
				throw Config::ConfigException("Invalid port value: " + tokens[i]);
			server.port = std::atoi(tokens[i++].c_str());
			listen_set = true;
		}
		else if (key == "host")
		{
			expect_value(tokens, i, key);
			if (host_set)
				throw Config::ConfigException("Duplicate 'host' directive");
			if (!is_valid_ip(tokens[i]))
				throw Config::ConfigException("Invalid host/IP: " + tokens[i]);
			server.host = tokens[i++];
			host_set = true;
		}
		else if (key == "server_name")
		{
			expect_value(tokens, i, key);
			server.server_name = tokens[i++];
		}
		else if (key == "root")
		{
			expect_value(tokens, i, key);
			if (root_set)
				throw Config::ConfigException("Duplicate 'root' directive");
			if (tokens[i].empty())
				throw Config::ConfigException("'root' can't be empty");
			server.root = tokens[i++];
			root_set = true;
		}
		else if (key == "client_max_body_size")
		{
			expect_value(tokens, i, key);
			if (client_max_body_size_set)
				throw Config::ConfigException("Duplicate 'client_max_body_size' directive");
			if (!is_number(tokens[i]))
				throw Config::ConfigException("Invalid client_max_body_size value");
			server.client_max_body_size = std::atoi(tokens[i++].c_str());
			client_max_body_size_set = true;
		}
		else if (key == "error_page")
		{
			expect_value(tokens, i, key);
			if (!is_number(tokens[i]))
				throw Config::ConfigException("Invalid error_page code: " + tokens[i]);
			int code = std::atoi(tokens[i++].c_str());
			if (code < 100 || code > 599)
				throw Config::ConfigException("error_page code must be between 100 and 599");
			expect_value(tokens, i, "error_page");
			if (tokens[i].empty())
				throw Config::ConfigException("error_page path can't be empty");
			server.error_pages[code] = tokens[i++];
		}
		else if (key == "location")
			server.locations.push_back(parse_location(tokens, i));
		else
			throw Config::ConfigException("Unknown server directive : " + key);

		if (i < tokens.size() && tokens[i] == ";")
			i++;
	}
	if (i >= tokens.size() || tokens[i] != "}")
		throw Config::ConfigException("Expected '}' to close server block");

	i++;

	//check all value
	if (!listen_set)
		throw Config::ConfigException("Server block missing 'listen' directive");

	if (server.port < 1 || server.port > 65535)
		throw Config::ConfigException("The server " + server.server_name + " has a wrong port number, must be between 1 and 65535");

	if (server.locations.empty())
		throw Config::ConfigException("Server block must define at least one 'location'");

	if (!host_set)
		server.host = "0.0.0.0";

	return server;
}
```

Pour chaque bloc "location", les directives possibles définissent un dossier racine propre à cette route, un fichier d'index, l'activation ou non du listing automatique de dossier, un emplacement de stockage pour les fichiers envoyés par l'utilisateur, une éventuelle redirection avec son code et son adresse cible, une association entre une extension de fichier et un programme externe chargé de l'exécuter, la liste des méthodes HTTP autorisées, ainsi qu'une taille maximale de corps de requête propre à cette route. Comme pour les serveurs, chaque valeur est contrôlée avant d'être acceptée : les codes de redirection doivent appartenir à la plage réservée à ce type de réponse, les méthodes HTTP doivent faire partie d'un ensemble limité et reconnu, et la plupart des directives ne peuvent être définies qu'une seule fois par route.

```
static Location parse_location(const std::vector<std::string> &tokens, size_t &i)
{
	Location loc;

	std::cout << "DEBUG : start location parsing" << std::endl;

	expect_value(tokens, i, "location");
	if (tokens[i].empty())
		throw Config::ConfigException("Location path can't ne empty");
	loc.path = tokens[i++]; //le path vient juste après "location"

	expect_value(tokens, i, "location path");
	if (tokens[i++] != "{")
		throw Config::ConfigException("Expected '{' after location path");
	
	bool root_set = false;
	bool index_set = false;
	bool autoindex_set = false;
	bool upload_store_set = false;
	bool redirect_set = false;
	bool methods_set = false;
	bool client_max_body_size_set = false;
	
	while (i < tokens.size() && tokens[i] != "}")
	{
		std::string key = tokens[i++];

		if (key == "root")
		{
			expect_value(tokens, i, key);
			if (root_set)
				throw Config::ConfigException("Duplicate 'root' directive in location " + loc.path);
			if (tokens[i].empty())
				throw Config::ConfigException("'root' can't be empty in location " + loc.path);
			loc.root = tokens[i++];
			root_set = true;
		}
		else if (key == "index")
		{
			expect_value(tokens, i, key);
			if (index_set)
				throw Config::ConfigException("Duplicate 'index' directive in location " + loc.path);
			loc.index = tokens[i++];
			index_set = true;
		}
		else if (key == "autoindex")
		{
			expect_value(tokens, i, key);
			if (autoindex_set)
				throw Config::ConfigException("Duplicate 'root' directive in location " + loc.path);
			if (tokens[i] != "on" && tokens[i] != "off")
				throw Config::ConfigException("'autoindex' must be 'on' or 'off' in location " + loc.path + ", got: " + tokens[i]);
			loc.autoindex = (tokens[i++] == "on");
			autoindex_set = true;
		}
		else if (key == "upload_store")
		{
			expect_value(tokens, i, key);
			if (upload_store_set)
				throw Config::ConfigException("Duplicate 'upload_store' directive in location " + loc.path);
			loc.upload_store = tokens[i++];
			upload_store_set = true;
		}
		else if (key == "return")
		{
			if (redirect_set)
				throw Config::ConfigException("Duplicate 'return' directive in location " + loc.path);

			expect_value(tokens, i, "return code");
			if (!is_number(tokens[i]))
				throw Config::ConfigException("Invalid return code: " + tokens[i]);
			int code = std::atoi(tokens[i++].c_str());
			if (code < 300 || code > 399)
				throw Config::ConfigException("return code muste be a valid redirect code (300-399)");
			
			expect_value(tokens, i, "return url");
			if (tokens[i].empty() || tokens[i] == ";")
				throw Config::ConfigException("return url can't be empty in location " + loc.path);
			
			loc.redirect_url = tokens[i++];
			loc.redirect_code = code;
			redirect_set = true;
		}
		else if (key == "cgi_pass")
		{
			expect_value(tokens, i, "cgi_pass extension");
			std::string ext = tokens[i++];
			expect_value(tokens, i, "cgi_pass path");
			std::string path = tokens[i++];

			if (loc.cgi_pass.count(ext))
				throw Config::ConfigException("Duplicate 'cgi_pass' for extension " + ext + " in location " + loc.path);
			
			loc.cgi_pass[ext] = path;
		}
		else if (key == "methods")
		{
			if (methods_set)
				throw Config::ConfigException("Duplicate 'methods' directive in location " + loc.path);
			while (i < tokens.size() && tokens[i] != ";")
			{
				const std::string &m = tokens[i];
				if (m != "GET" && m != "POST" && m != "DELETE")
					throw Config::ConfigException("Unsupported HTTP method: " + m);
				loc.methods.push_back(tokens[i++]);
			}
			if (loc.methods.empty())
				throw Config::ConfigException("'methods' directive can't be empty in location " + loc.path);
			methods_set = true;
		}
		else if (key == "client_max_body_size")
		{
			expect_value(tokens, i, key);
			if (client_max_body_size_set)
				throw Config::ConfigException("Duplicate 'client_max_body_size' directive");
			if (!is_number(tokens[i]))
				throw Config::ConfigException("Invalid client_max_body_size value");
			loc.client_max_body_size = std::atoi(tokens[i++].c_str());
			client_max_body_size_set = true;
		}
		else
			throw Config::ConfigException("Unknown location directive: " + key);

		if (i < tokens.size() && tokens[i] == ";")
			i++;
	}
	if (i >= tokens.size() || tokens[i] != "}")
		throw Config::ConfigException("Expected '}' to close location block");
	i++;
	return loc;
}
```

Dans les deux cas, toute directive qui ne correspond à aucun des mots-clés reconnus provoque une erreur explicite, ce qui évite de laisser passer silencieusement une faute de frappe ou une directive mal placée dans le fichier de configuration.

### Validation des règles qui dépendent de plusieurs blocs

Une fois que tous les blocs "serveur" et "location" ont été entièrement construits, une dernière passe de vérification est effectuée sur l'ensemble de la configuration. Elle traite les règles qui ne peuvent pas être contrôlées directement pendant la lecture d'un seul bloc, parce qu'elles dépendent de la relation entre une route et le serveur auquel elle appartient.

La première règle concerne le dossier racine : si une route ne définit pas son propre dossier racine et que le serveur auquel elle appartient n'en définit pas non plus, la configuration est jugée incomplète et une erreur est levée, puisqu'aucune valeur de repli ne serait disponible pour savoir où chercher les fichiers à servir.

La seconde règle concerne la taille maximale de corps de requête : si une route n'a pas précisé sa propre limite, elle hérite automatiquement de la limite définie au niveau du serveur parent, ce qui évite d'avoir à répéter cette valeur dans chaque route si elle est la même partout.

```
static void validate_inheritance(std::vector<ServerBlock> &servers)
{
	for (size_t i = 0; i < servers.size(); ++i)
	{
		ServerBlock &server = servers[i];

		for(size_t j = 0; j < server.locations.size(); ++j)
		{
			Location &loc = server.locations[j];

			if (loc.root.empty() && server.root.empty())
				throw Config::ConfigException("No 'root' defined for location \"" + loc.path + "\" and no server-level fallback");

			if (loc.client_max_body_size == 0)
				loc.client_max_body_size = server.client_max_body_size;
		}
	}
}
```

---

### Lancer le site web

Il peut arriver que lancer simplement http://localhost:8080/ ne fonctionne pas car le projet tourne sur WSL et le navigateur web sous Windows.
Sous WSL faire la commande "hostname -I" puis récupérer l'adresse IP fournie et l'écrire à la place de localhost -> http://xxx.xxx.xxx:8080/

