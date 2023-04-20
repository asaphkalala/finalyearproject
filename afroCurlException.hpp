#ifndef afroCurl_afroCurl_EXCEPTION_HPP
#define afroCurl_afroCurl_EXCEPTION_HPP

#include <stdexcept>

namespace afroCurl
{

///-------------------------------------------------------------------------------------------------
/// Exception for signalling afroCurl errors. 
///-------------------------------------------------------------------------------------------------
class afroCurlException : public std::exception
{
public:
	///-------------------------------------------------------------------------------------------------
	/// Constructor. 
	///
	/// \param	aMessage	Message describing exception. 
	///-------------------------------------------------------------------------------------------------
	afroCurlException( const char * const & aMessage ): exception( aMessage ) 
	{
	}
};
	
} //namespace afroCurl

#endif // afroCurl_afroCurl_EXCEPTION_HPP
