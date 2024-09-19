
#ifndef MESSAGE_HPP
# define MESSAGE_HPP

# include <iostream>
# include <list>
# include <sstream>

class Node;
class RequestHeader;
class AHeader;

class Message {

	public:
									// canonical
									Message(bool r);
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
		const std::string			getUnchunkedBodyString();
		const std::list<Node>::iterator& 		getIterator();	 // linked list of nodes
		void						_createHeader();
		void						_headerInfoToNode();

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

		void						_isNodeComplete();
		void						_parseNode();
		void						_addNewNode();
		size_t						_calcChunkSize(std::string s);
		void						_findBody(std::list<Node>::iterator& it);
		size_t						_calcOptimalChunkSize(std::list<Node>::iterator& it);
		Node						_newChunkNode(size_t size);
		std::string					_createCgiHeaderDel();
		// void						_createHeader();
		// void						_headerInfoToNode();

									Message(void);
									Message(Message const & src);
		Message &					operator=(Message const & rhs);
};

#endif
