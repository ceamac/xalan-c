// Base header file.  Must be first.
#include <xalanc/Include/PlatformDefinitions.hpp>



#include <cassert>



#if defined(XALAN_CLASSIC_IOSTREAMS)
#include <fstream.h>
#include <iostream.h>
#include <strstream.h>
#else
#include <fstream>
#include <iostream>
#include <strstream>
#endif



#include <xercesc/util/PlatformUtils.hpp>



#include <xalanc/XalanTransformer/XalanTransformer.hpp>



int
main(
			int				argc,
			const char*		/* argv */[])
{
	XALAN_USING_STD(cerr)
	XALAN_USING_STD(endl)
	XALAN_USING_STD(ostrstream)

	int	theResult = 0;

	if (argc != 1)
	{
		cerr << "Usage: CompileStylesheet"
			 << endl
			 << endl;
	}
	else
	{
		XALAN_USING_XERCES(XMLPlatformUtils)

		XALAN_USING_XALAN(XalanTransformer)
		XALAN_USING_XALAN(XalanCompiledStylesheet)

		// Call the static initializer for Xerces.
		XMLPlatformUtils::Initialize();

		// Initialize Xalan.
		XalanTransformer::initialize();

		{
			// Create a XalanTransformer.
			XalanTransformer theXalanTransformer;

			// Our input files...The assumption is that the executable will be run
			// from same directory as the input files.
			const char*		theXSLFileName = "foo.xsl";
			
			// Compile the stylesheet.
			const XalanCompiledStylesheet*	theCompiledStylesheet = 0;

			theResult =	theXalanTransformer.compileStylesheet(theXSLFileName, theCompiledStylesheet);

			if (theResult == 0)
			{
				assert(theCompiledStylesheet != 0);

				for (unsigned int i = 0; i < 10; ++i)
				{		
					// Buffers passed in to ostrstream.
					char	inBuffer[10];
					char	outBuffer[10];	

					// Generate the input and output file names.
					ostrstream	theFormatterIn(inBuffer, sizeof(inBuffer));
					ostrstream	theFormatterOut(outBuffer, sizeof(outBuffer));

					theFormatterIn << "foo" << i + 1 << ".xml" << '\0';
					theFormatterOut << "foo" << i + 1 << ".out" << '\0';

					// Do the transform.
					theResult = theXalanTransformer.transform(
							inBuffer,
							theCompiledStylesheet,
							outBuffer);
				}

				// I don't have to do this, since the transformer will
				// clean this up when it's destroyed, but why not...
				theResult = theXalanTransformer.destroyStylesheet(theCompiledStylesheet);
			}
   
			if(theResult != 0)
			{
				cerr << "CompileStylesheet Error: " << theXalanTransformer.getLastError()
					 << endl
					 << endl;
			}
		}

		// Terminate Xalan...
		XalanTransformer::terminate();

		// Terminate Xerces...
		XMLPlatformUtils::Terminate();

		// Clean up the ICU, if it's integrated.
		XalanTransformer::ICUCleanUp();
	}

	return theResult;
}
