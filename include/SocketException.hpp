#if !defined(__net_SocketException)
#define __net_SocketException

#include <cerrno>
#include <iostream>


namespace net
{
	class SocketException {
		public:
			SocketException(std::string call, std::string event):
				m_call(call), m_event(event) { }
			SocketException(std::string call, const int errval):
				m_call(call),
				m_errno(errval) { }

			void display(void) const {
				m_errno ?
					std::cout << "[*] " << m_call << ": "
						<< std::strerror(m_errno) << '\n'
					:
					std::cout << "[*] " << m_call << ": "
						<< m_event << '\n';
			}

			int logError(void);
		private:
			std::string m_event;
			std::string m_call;
			int m_errno;
	};

	/* INcludes the default behavior, and adds features for the server*/
	std::string& strerror(int _errno);
};
#endif
