#include "XPathWrapper.hpp"



#include <xercesc/util/XMLException.hpp>



#include <xalanc/PlatformSupport/DOMStringHelper.hpp>



#if defined(XALAN_CLASSIC_IOSTREAMS)
#include <fstream.h>
#include <iostream.h>
#else
#include <fstream>
#include <iostream>
#endif



int
main(
			int				argc,
			const char*		argv[])
{
	XALAN_USING_STD(cerr)
	XALAN_USING_STD(cout)
	XALAN_USING_STD(endl)
	XALAN_USING_STD(ifstream)

	if (argc < 4)
	{
		cerr << "Usage: TestDriver XMLFilePath Context XPathExpression" << endl;
		return -1;
	}

	XPathWrapper::ChVectorType	theXML;

	ifstream	in(argv[1]);

	// slow and dirty dump of the xml file into a buffer
	char c;
	while(in.get(c))
		theXML.push_back(c);
	theXML.push_back('\0');

	///////////////////////////////////////////..

	// create your XPath helper object
	XPathWrapper helper;

	try
	{
		// call evaluate, passing in the XML string, the context string and the xpath string
		const XPathWrapper::CharVectorTypeVectorType	result = helper.evaluate(&*theXML.begin(), argv[2], argv[3]);

		// take the resulting string vector	and do whatever you want with it:
		size_t len = result.size();

		cout << "the result set has " << len << " strings\n";

		for (size_t i=0; i<len; i++)
			cout << "item " << (i+1) << "= \"" << &*result[i].begin() << "\"" << endl;
	}
	catch(const XERCES_CPP_NAMESPACE_QUALIFIER XMLException&)
	{
		cerr << "Exception caught!  Exiting..." << endl;
	}

	return 0;
}
