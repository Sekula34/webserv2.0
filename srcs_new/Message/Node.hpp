
#ifndef NODE_HPP
# define NODE_HPP


# define	HEADER		0
# define	BODY		1
# define	CHUNK		2
# define	LCHUNK		3
# define	TRAILER		4

# define	INCOMPLETE	0
# define	COMPLETE	1
# define	ERROR		-1

# include <iostream>
# include <sstream>

class Node {

	public:
							// canonical
							Node (const std::string & str, int type, bool r);
							Node (const std::string & str, int type, size_t size, bool r);
							~Node(void);
							Node(void);
							Node(Node const & src);

		void				concatString(char* buffer, size_t & bufferPos, size_t num);

							// set and get
		void				setString(const std::string & s);
		void				setStringChunked(const std::string & s);
		void				setState(int state);
		std::string			getStringUnchunked();
		std::string			getStringChunked();
		const std::string&	getString() const;
		const int &			getType() const;
		const int &			getState() const;
		bool				getChunkHeader() const;
		size_t				getChunkSize() const;
		size_t				getBodySize() const;
		size_t				getChunkHeaderSize();
		bool				getHasCgiDel();
		void				setType(int type);
		void				setChunkHeader(bool b);
		void				setBodySize(const size_t size);
		void				setChunkSize(const size_t size);
		void				setChunkHeaderSize(const size_t size);


	private:
		int					_state; // Node can be COMPLETE or INCOMPLETE
		int					_type; // could be of type: HEADER, BODY, CHUNK, LCHUNK, TRAILER
		std::string 		_str; // the content of message in node
		size_t				_btr; // bytes to read from the buffer
		size_t				_chunkSize; // the size of of CHUNK BODY
		size_t				_chunkHeaderSize; // size of CHUNK HEADER
		size_t				_bodySize; // if this node is a BODY this will be it's size
		bool				_chunkHeader; // flag to indicate that we have set chunkSize
		bool				_request;
		bool				_hasNewline;
		bool				_hasCgiDel;
		// std::stringstream 	_ss;

							//Node specific functions
		std::string			_getRemainDel(const std::string & del);
		std::string			_chunk();
		std::string			_unChunk();
		bool				_checkRemainDelIsBufStart(std::string remainDel, char* buffer, size_t bufferPos);
		size_t				_calcBtr(char* buffer, std::string del, size_t & bufferPos, size_t num);
		size_t				_calcBtrCgi(char* buffer, size_t & bufferPos, size_t num);
		void				_setBtr(char* buffer, size_t & bufferPos, size_t num);

		Node &				operator=(Node const & rhs);
};

#endif
