// gcc -o parser "dgf_parser.cpp" -lxml2
#include <stdio.h>
#include <libxml/HTMLparser.h>
#include <libxml/xpath.h>

// steps:
// process all children, add pq
// pop pq until empty; write to csv

void processAllChildren(xmlNodePtr parent) {
    // add pq to this
    if (parent == NULL) {
        fprintf(stderr, "Error: Parent node is NULL.\n");
        return;
    }

    printf("Direct children of <%s>:\n", parent->name);

    for (xmlNodePtr child = parent->children; child; child = child->next) {
        // Ensure the child is an element node
        if (child->type == XML_ELEMENT_NODE) {
            printf("  <%s>: %s\n", child->name, (char *)xmlNodeGetContent(child));
            // printf("first td: %s\n", (char *)xmlNodeGetContent(child));
            printf("first td: %s\n", (char *)xmlNodeGetContent(child -> children));
            printf("second td: %s\n", (char *)xmlNodeGetContent(child -> children -> next));
            printf("third td: %s\n", (char *)xmlNodeGetContent(child -> children -> next -> next));
            printf("fourth td: %s\n", (char *)xmlNodeGetContent(child -> children -> next -> next -> next));
            // printf("first td: %s\n", (char *)xmlNodeGetContent(child -> next -> next));
            // printDirectChildren(child);
        }
    }
}

int main(int argc, char *argv[]) {

    // Parse the HTML file
    htmlDocPtr doc = htmlReadFile("sample.html", NULL, HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);
    if (doc == NULL) {
        fprintf(stderr, "Error: Could not parse file\n");
        return 1;
    }

    // XPath to find the <table> element
    xmlXPathContextPtr xpathCtx = xmlXPathNewContext(doc);
    if (xpathCtx == NULL) {
        fprintf(stderr, "Error: Unable to create XPath context\n");
        xmlFreeDoc(doc);
        return 1;
    }

    const char *xpathExpr = "/html/body/div[1]/div/div/main/div/div[2]/div/div/div/table/tbody";
    xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression((const xmlChar *)xpathExpr, xpathCtx);
    if (xpathObj == NULL) {
        fprintf(stderr, "Error: Unable to evaluate XPath expression\n");
        xmlXPathFreeContext(xpathCtx);
        xmlFreeDoc(doc);
        return 1;
    }

    xmlNodeSetPtr nodes = xpathObj->nodesetval;
    if (nodes && nodes->nodeNr > 0) {
        xmlNodePtr table = nodes->nodeTab[0];
        processAllChildren(table);
    } else {
        printf("No <table> found for the XPath query: %s\n", xpathExpr);
    }

    // Cleanup
    xmlXPathFreeObject(xpathObj);
    xmlXPathFreeContext(xpathCtx);
    xmlFreeDoc(doc);
    xmlCleanupParser();

    return 0;
}