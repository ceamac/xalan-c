/*
 * The Apache Software License, Version 1.1
 *
 *
 * Copyright (c) 1999 The Apache Software Foundation.  All rights 
 * reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The end-user documentation included with the redistribution,
 *    if any, must include the following acknowledgment:  
 *       "This product includes software developed by the
 *        Apache Software Foundation (http://www.apache.org/)."
 *    Alternately, this acknowledgment may appear in the software itself,
 *    if and wherever such third-party acknowledgments normally appear.
 *
 * 4. The names "Xalan" and "Apache Software Foundation" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written 
 *    permission, please contact apache@apache.org.
 *
 * 5. Products derived from this software may not be called "Apache",
 *    nor may "Apache" appear in their name, without prior written
 *    permission of the Apache Software Foundation.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE APACHE SOFTWARE FOUNDATION OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the Apache Software Foundation and was
 * originally based on software copyright (c) 1999, International
 * Business Machines, Inc., http://www.ibm.com.  For more
 * information on the Apache Software Foundation, please see
 * <http://www.apache.org/>.
 *
 * $ Id: $
 *
 */

#include "ExtensionNSHandler.hpp"



#include <cassert>


#include <XalanDOM/XalanElement.hpp>



#include <PlatformSupport/StringTokenizer.hpp>
#include <PlatformSupport/DOMStringHelper.hpp>



#include "StylesheetExecutionContext.hpp"



class XSLTProcessor;



struct XSLProcessorContext
{
	const XSLTProcessor&	m_processor;
	const Stylesheet&		m_stylesheetTree;
	const XalanNode*		m_sourceTree;
	const XalanNode*		m_sourceNode;
	const XalanQName&		m_mode;

	XSLProcessorContext(
			const XSLTProcessor&	processor,
			const Stylesheet&		stylesheetTree, 
			const XalanNode*		sourceTree,
			const XalanNode*		sourceNode,
			const XalanQName&		mode) :
		m_processor(processor),
		m_stylesheetTree(stylesheetTree),
		m_sourceTree(sourceTree),
		m_sourceNode(sourceNode),
		m_mode(mode)
	{
	}
};



/**
 * Construct a new extension namespace handler for a given extension NS.
 * This doesn't do anything - just hang on to the namespace URI.
 * 
 * @param namespaceUri the extension namespace URI that I'm implementing
 */
ExtensionNSHandler::ExtensionNSHandler(const XalanDOMString&	namespaceUri) :
	ExtensionFunctionHandler(namespaceUri),
	m_elements(),
	m_componentDescLoaded(false)
{
}



/**
 * Construct a new extension namespace handler given all the information
 * needed. 
 * 
 * @param xslp				 handle to the XSL processor that I'm working for
 * @param namespaceUri the extension namespace URI that I'm implementing
 * @param elemNames		string containing list of elements of extension NS
 * @param funcNames		string containing list of functions of extension NS
 * @param lang				 language of code implementing the extension
 * @param srcURL			 value of src attribute (if any) - treated as a URL
 *										 or a classname depending on the value of lang. If
 *										 srcURL is not null, then scriptSrc is ignored.
 * @param scriptSrc		the actual script code (if any)
 */
ExtensionNSHandler::ExtensionNSHandler (
			const XalanDOMString& namespaceUri,
			const XalanDOMString& elemNames,
			const XalanDOMString& funcNames,
			const XalanDOMString& lang,
			const XalanDOMString& srcURL,
			const XalanDOMString& src) :
	ExtensionFunctionHandler(namespaceUri, funcNames, lang, srcURL, src),
	m_elements(),
	m_componentDescLoaded(true)
		 
{
	setElements(elemNames);
}



void
ExtensionNSHandler::setFunctions(const XalanDOMString&	funcNames)
{
    ExtensionFunctionHandler::setFunctions(funcNames);

    m_componentDescLoaded = true;
}



void
ExtensionNSHandler::setScript(
			const XalanDOMString&	lang,
			const XalanDOMString&	srcURL,
			const XalanDOMString&	scriptSrc)
{
    ExtensionFunctionHandler::setScript(lang, srcURL, scriptSrc);

    m_componentDescLoaded = true;
}



void
ExtensionNSHandler::setElements(const XalanDOMString&	elemNames)
{
    if (length(elemNames) != 0)
	{
		StringTokenizer		st(elemNames, s_tokenDelimiterCharacters, false);

		while (st.hasMoreTokens() == true)
		{
			m_elements.insert(st.nextToken()); // just stick it in there basically
		}
	
		m_componentDescLoaded = true;
	}
}



bool
ExtensionNSHandler::isElementAvailable(const XalanDOMString&	element) const
{
    return (m_elements.find(element) != m_elements.end());
}



void
ExtensionNSHandler::processElement(
			StylesheetExecutionContext&		executionContext,
			const XalanDOMString&			localPart,
			const XalanElement*				/* element */,
			Stylesheet&						/* stylesheetTree */, 
			const XalanNode*				/* sourceTree */,
			const XalanNode*				/* sourceNode */,
			const XalanQName&				/* mode */)
{
	XObjectPtr	result;

	if (!m_componentStarted) 
	{
		try 
		{
			startupComponent();

			ArgVectorType	argv;
			
//			XSLProcessorContext xpc(processor,
//					stylesheetTree, sourceTree, sourceNode, mode);
			/*
				java:
				Vector argv = new Vector (2);
				argv.addElement (xpc);
				argv.addElement (element);
			*/
				// $$$ ToDo: There's no way this will ever work...
//				argv.push_back(static_cast<void *>(&xpc));
//				argv.push_back(static_cast<void *>(&element));
			result = ExtensionFunctionHandler::callFunction (localPart, argv);
		}
		// catch (XPathProcessorException e) 
		catch (...) 
		{
			/*
				e.printStackTrace ();
				throw new XSLProcessorException (e.getMessage (), e);
			*/
			//@@ TODO: Error reporting, or throw
		} 
	}

	if (result.null() == false) 
	{
		executionContext.outputToResultTree(*result, 0);
	}
}



void
ExtensionNSHandler::startupComponent()
{
	if (!m_componentDescLoaded) 
	{
		try 
		{
			loadComponentDescription();
		}
		catch (...) 
		// catch (Exception e) 
		{
			// throw new XPathProcessorException (e.getMessage (), e);
			//@@ TODO: Error reporting, or throw
		}
	}

	ExtensionFunctionHandler::startupComponent();
}



void
ExtensionNSHandler::loadComponentDescription()
{
	// First try treaing the URI of the extension as a fully qualified
	// class name; if it works then go with treating this an extension
	// implemented in "javaclass" for with that class being the srcURL.
	// forget about setting elements and functions in that case - so if
	// u do extension-{element,function}-available then u get false, 
	// but that's ok.
	assert(0);		// Don't try this in C++
	// @@ How to do this in C++ ??
/*
	try {
		String cname = namespaceUri.startsWith ("class:") ?
										 namespaceUri.substring (6) : namespaceUri;
		Class.forName (cname); // does it load?
		setScript ("javaclass", namespaceUri, null);
		m_componentDescLoaded = true;
		return;
	} catch (Exception e) {
		// oops, it failed .. ok, so this path ain't gonna pan out. shucks.
	}

	// parse the document at the URI of the extension, if any
	URL url = m_XSLProcessor.getURLFromString(namespaceUri,
																	m_XSLProcessor.m_stylesheetRoot.m_baseIdent);
	
	m_XSLProcessor.m_parserLiaison.parse(new InputSource(url.toString()));
	Document compSpec = m_XSLProcessor.m_parserLiaison.getDocument();
	Element componentElement = compSpec.getDocumentElement ();

	// determine the functions and elements of this component
	setElements (componentElement.getAttribute ("elements"));
	setFunctions (componentElement.getAttribute ("functions"));

	// is there an lxslt:script element child? [NOTE THAT THIS IS NOT
	// PROPER NAMESPACE-WISE .. I'll FIX IT LATER. .. Sanjiva 8/20/99.]
	NodeList nl = componentElement.getElementsByTagName ("lxslt:script");
	switch (nl.getLength ()) 
	{
	case 0:
		break;
	case 1:
		Element scriptElem = (Element) nl.item (0);
		String lang = scriptElem.getAttribute ("lang");
		Attr srcURLAttr = scriptElem.getAttributeNode ("src");
		String srcURL = (srcURLAttr == null) ? null : srcURLAttr.getValue ();
		String src = getScriptString (scriptElem);
		setScript (lang, srcURL, src);
		break;
	default:
		throw new XSLProcessorException ("too many <script>s in component");
	}
	m_componentDescLoaded = true;
*/
}



XalanDOMString
ExtensionNSHandler::getScriptString(const XalanElement&		elem)
{
	XalanDOMString strBuf;

	for (const XalanNode*	n = elem.getFirstChild (); n != 0; n = n->getNextSibling())
	{
		switch (n->getNodeType()) 
		{
		case XalanNode::TEXT_NODE:
		case XalanNode::CDATA_SECTION_NODE:
			strBuf += n->getNodeValue();
			break;

		default:
			break;
		}
	}

	return strBuf;
}
