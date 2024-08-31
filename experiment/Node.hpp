
#ifndef NODE_HPP
# define NODE_HPP

# define	MAXLINE		40

# define	HEADER		0
# define	BODY		1
# define	CHUNK		2
# define	TRAIL		3

# define	INCOMPLETE	0
# define	COMPLETE	1
# define	ERROR		-1

# include <iostream>

class Node {

	public:
							// canonical
							Node (const std::string & str, int type, size_t bufferPos);
							~Node(void);
							Node(void);
							Node(Node const & src);

							// set and get
		void				setString(const std::string & s);
		void				setState(int state);
		void				concatString(char* buffer, size_t & bufferPos, size_t num);
		std::string			getStringUnchunked();
		std::string			getStringChunked();
		const int &			getType() const;
		const int &			getState() const;

							//Node specific functions

	private:
		int					_state;
		const int			_type;
		std::string 		_str;
		size_t				_bufferPos;
		size_t				_btr;

		std::string			_chunk();
		std::string			_unChunk();
		std::string			_getRemainDel(const std::string & del);
		bool				_checkRemainDelIsBufStart(std::string remainDel, char* buffer);
		void				_calcBtr(char* buffer, std::string del, size_t & bufferPos, size_t num);

		Node &				operator=(Node const & rhs);
};

#endif

