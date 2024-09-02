
#ifndef NODE_HPP
# define NODE_HPP

#ifndef MAXLINE
# define	MAXLINE		20
#endif

# define	HEADER		0
# define	BODY		1
# define	CHUNK		2
# define	LCHUNK		3
# define	TRAILER		4

# define	INCOMPLETE	0
# define	COMPLETE	1
# define	ERROR		-1

# include <iostream>

class Node {

	public:
							// canonical
							Node (const std::string & str, int type);
							~Node(void);
							Node(void);
							Node(Node const & src);

		void				concatString(char* buffer, size_t & bufferPos, size_t num);

							// set and get
		void				setString(const std::string & s);
		void				setState(int state);
		std::string			getStringUnchunked();
		std::string			getStringChunked();
		const int &			getType() const;
		const int &			getState() const;
		bool				getChunkHeader() const;
		size_t				getChunkSize() const;
		void				setType(int type);
		void				setChunkHeader(bool b);
		void				setBodySize(const size_t size);
		void				setChunkSize(const size_t size);
		void				setChunkHeaderSize(const size_t size);


	private:
		int					_state;
		int					_type;
		std::string 		_str;
		size_t				_btr;
		size_t				_chunkSize;
		size_t				_chunkHeaderSize;
		size_t				_bodySize;
		bool				_chunkHeader;

							//Node specific functions
		std::string			_chunk();
		std::string			_unChunk();
		std::string			_getRemainDel(const std::string & del);
		bool				_checkRemainDelIsBufStart(std::string remainDel, char* buffer, size_t bufferPos);
		void				_calcBtr(char* buffer, std::string del, size_t & bufferPos, size_t num);
		void				_setBtr(char* buffer, size_t & bufferPos, size_t num);

		Node &				operator=(Node const & rhs);
};

#endif

