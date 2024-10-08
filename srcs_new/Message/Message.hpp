
#ifndef MESSAGE_HPP
# define MESSAGE_HPP

# include <list>
# include <sstream>
# include <string>
# include "AHeader.hpp"

# define MAX_BODY_SIZE	20
# define MAX_CHUNKSIZE	30

class Node;
class RequestHeader;
class ResponseHeader;

class Message {

	public:
		// Methods
		void						bufferToNodes(char* buffer, size_t num);
		void						printChain();
		void						stringsToChain(ResponseHeader* header, const std::string& body);
		void						createHeader();
		void						advanceIterator();
		void						resetIterator();
		int							getState() const;
		AHeader*					getHeader() const;
		std::list<Node>&			getChain();
		const std::string			getBodyString();
		int&						getErrorCode() const;
		const size_t&				getBytesSent() const;
		void						setState(int s);
		void						setBytesSent(size_t num);
		const std::list<Node>::iterator& 		getIterator();	 // linked list of nodes

									//Message specific functions
	private:
		// Methods
		void						_headerInfoToNode();
		void						_chunksToBody();
		void						_bodyToChunks(const std::string& body);
		void						_setNodeComplete();
		void						_parseNode();
		void						_addNewNode();
		size_t						_calcChunkSize(std::string s);
		void						_findBody(std::list<Node>::iterator& it);
		size_t						_calcOptimalChunkSize(const std::string& body);
		Node						_newChunkNode(size_t size);
		std::string					_createCgiHeaderDel();

		// Attributes
		std::list<Node> 			_chain;		// linked list of nodes
		std::list<Node>::iterator	_it;		// pointing to current node in chain
		bool						_chunked;	// this is a chunked request
		bool						_trailer;	// we are expecting a trailer as last node
		int							_state;		// message can be: COMPLETE, INCOMPLETE, ERROR
		bool						_request;
		std::stringstream 			_ss;
		AHeader*					_header;
		int&						_errorCode;
		size_t						_bytesSent;


	public:
									Message(bool request, int& errorCode);
									~Message(void);

	private:
									Message(Message const & src);
		Message &					operator=(Message const & rhs);
};

#endif
