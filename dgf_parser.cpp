// gcc -o parser "dgf_parser.cpp" -lxml2
#include <stdio.h>
#include <libxml/HTMLparser.h>
#include <libxml/xpath.h>

// steps:
// process all children, add pq
// pop pq until empty; write to csv

void processChild(xmlNodePtr child) {
    // pq of pairs
    child = child -> next;
    printf("market name: %s\n", (char *)xmlNodeGetContent(child));
    // add this as the pq key, or whatever
    for (int i = 0; i < 5; i++)
        child = child -> next;
    printf("fliff odds: %s\n", (char *)xmlNodeGetContent(child));
    // calculate
    for (int i = 0; i < 5; i++)
        child = child -> next;
    if (child -> children -> children) {
        printf("FanDuel odds: %s\n", (char *)xmlNodeGetContent(child -> children -> children -> children -> children -> next));
    }
    // calculate
    child = child -> next;
    if (child -> children -> children) {
        printf("DraftKings odds: %s\n", (char *)xmlNodeGetContent(child -> children -> children -> children -> children -> next));
    }
    // calculate

    // this seems like a really goon way to traverse a linked list but idk if there's a better way
}

void processAllChildren(xmlNodePtr parent) {
    // add pq to this
    if (parent == NULL) {
        fprintf(stderr, "Error: Parent node is NULL.\n");
        return;
    }
    
    for (xmlNodePtr child = parent->children; child; child = child->next) {
        // Ensure the child is an element node
        if (child->type == XML_ELEMENT_NODE) {
            printf("  <%s>: %s\n", child->name, (char *)xmlNodeGetContent(child));
            // printf("first td: %s\n", (char *)xmlNodeGetContent(child));
            processChild(child -> children); // name
            // note: this is redundant code. should probably wrap this in a helper.
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