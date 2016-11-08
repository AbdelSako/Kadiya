/*
MIT License

Copyright (c) 2016 Abdel Sako

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

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
