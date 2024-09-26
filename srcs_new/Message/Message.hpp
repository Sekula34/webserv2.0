
#ifndef MESSAGE_HPP
# define MESSAGE_HPP

# include <list>
# include <sstream>
# include "AHeader.hpp"

class Node;
class RequestHeader;

class Message {

	public:
									// canonical
									Message(bool request, int& errorCode);
		virtual						~Message(void);
		void						bufferToNodes(char* buffer, size_t num);
		void						printChain();
		void						_bodyToChunks();
		void						_chunksToBody();

									// set and get
		int							getState() const;
		void						setState(int s);
		AHeader*					getHeader() const;
		const std::list<Node>&		getChain() const;
		const std::string			getBodyString();
		const std::list<Node>::iterator& 		getIterator();	 // linked list of nodes
		void						_createHeader();
		void						_headerInfoToNode();
		const size_t&				getBytesSent() const;
		const size_t&				getBytesReceived() const;
		void						setBytesReceived(size_t num);
		void						setBytesSent(size_t num);

									//Message specific functions
	private:
		std::list<Node> 			_chain;	 // linked list of nodes
		std::list<Node>::iterator	_it;	// pointing to current node in chain
		bool						_chunked; // this is a chunked request
		bool						_trailer; // we are expecting a trailer as last node
		int							_state; // message can be: COMPLETE, INCOMPLETE, ERROR
		bool						_request;
		std::stringstream 			_ss;
		AHeader*					_header;
		int&						_errorCode;
		size_t						_bytesSent;
		size_t						_bytesReceived;

		void						_setNodeComplete();
		void						_parseNode();
		void						_addNewNode();
		size_t						_calcChunkSize(std::string s);
		void						_findBody(std::list<Node>::iterator& it);
		size_t						_calcOptimalChunkSize(std::list<Node>::iterator& it);
		Node						_newChunkNode(size_t size);
		std::string					_createCgiHeaderDel();
		// void						_createHeader();
		// void						_headerInfoToNode();

									//Message(void);
									Message(Message const & src);
		Message &					operator=(Message const & rhs);
};

#endif
