//FException

#ifndef _FEXCEPTION_H_
#define _FEXCEPTION_H_


namespace MeshLib
{

class FException 
{
 public:
	 FException();
	 FException(char * strg);

	 ~FException();

	 void what();
    
private:
 
	const char * msg;
};


}//end of namespace MeshLib
#endif //FException

