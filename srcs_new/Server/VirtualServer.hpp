#ifndef VIRTUALSERVER_HPP
# define VIRTUALSERVER_HPP

# include <iostream>
// TODO: Change dummy classes to the real ones one implemented.

class DummyServerSettings;
class DummyClient;
class DummyMessage;

class VirtualServer
{
	public:
		// Methods
		void	generateResponse(DummyClient& client);
		// void	generateError(Client& client); // Maybe this is not necessary, since genResponse could generate error.
		// Attributes

	private:
		// Methods
		bool	_isvalidRequest(DummyMessage* request);
		void	_execGet(DummyMessage* request);
		void	_execPost(DummyMessage* request);
		void	_execDelete(DummyMessage* request);
		// bool			validateHeaderAgainstSettings(DummyMessage* request);
		// Attributes
		const DummyServerSettings&		_serverSettings;
		// Maybe a static vector of virtual servers. like the one for Sockets.

	public:
		VirtualServer(const DummyServerSettings& settings);
		~VirtualServer();
		VirtualServer(const VirtualServer& src);

	private:
		// VirtualServer();
		VirtualServer&		operator=(const VirtualServer& src);

	// Overloaded insertion operator (for testing).
	friend std::ostream&	operator<<(std::ostream& out, const VirtualServer& serverSettings);
};

#endif
