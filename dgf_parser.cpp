#include <stdio.h>
#include <libxml/HTMLparser.h>
#include <libxml/xpath.h>
#include <cstdlib>
#include <utility>
#include <vector>
#include <unordered_map>
// steps:
// process all children by adding to vector, write to csv

// define some minimum threshold
// read from json file for vig data?

#define THRESHOLD = -20

std::unordered_map<int, int> dkVig;

void initDkMap() {
    FILE* fp = fopen("dkvig.bin", "rb");
    if (!fp) {
        perror("Could not open file");
        exit(1);
    }
    int key, value;
    while (fread(&key, sizeof(int), 1, fp) == 1 && fread(&value, sizeof(int), 1, fp) == 1) {
        dkVig[key] = value;
        dkVig[value] = key;
    }
    fclose(fp);
}

void processChild(xmlNodePtr child, std::vector<std::pair<int, char*> >& data) {
    child = child -> next;
    int bestOdds = INT_MIN;
    char* marketName = (char*)xmlNodeGetContent(child); // 1st index
    for (int i = 0; i < 5; i++)
        child = child -> next;
    int fliffOdds = atoi((char *)xmlNodeGetContent(child)); // 6th index
    for (int i = 0; i < 5; i++)
        child = child -> next;
    // pinnacle would be the 10th index
    if (child -> children -> children) {
        int fdOdds = atoi((char *)xmlNodeGetContent(child -> children -> children -> children -> children -> next)); // 11th index
        if (fdOdds < bestOdds)
            bestOdds = fdOdds;
    }
    child = child -> next;
    if (child -> children -> children) {
        int dkOdds = atoi((char *)xmlNodeGetContent(child -> children -> children -> children -> children -> next)); // 12th index
        if (dkVig.count(dkOdds)) {
            if (dkVig[dkOdds] > bestOdds)
            bestOdds = dkVig[dkOdds];
        } else {
            printf("Corresponding odds not found: %d\n", dkOdds);
        }
    }
    if (bestOdds != INT_MIN)
        printf("market name: %s\nfliff odds: %d\nbest odds: %d\n", marketName, fliffOdds, bestOdds);
    // this seems like a really goon way to traverse a linked list but idk if there's a better way
}

// Custom read function to read from a FILE pointer
int fileReadCallback(void *context, char *buffer, int len) {
    FILE *file = (FILE *)context;
    return fread(buffer, 1, len, file);
}

// Custom close function to close the FILE pointer
int fileCloseCallback(void *context) {
    FILE *file = (FILE *)context;
    return fclose(file);
}

int main() {
    htmlDocPtr doc = NULL;
    FILE *fp = fopen("sample.html", "r");
    fseek(fp, 0, SEEK_END);
    if (ftell(fp)) {
        puts("sample.html is non empty; reading from file...");
        doc = htmlReadFile("sample.html", NULL, HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);
    } else {
        puts("sample.html is empty; reading from clipboard...");
        fp = popen("pbpaste", "r");
        doc = htmlReadIO(fileReadCallback, fileCloseCallback, fp, NULL, NULL, HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);
    }
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

    initDkMap();
    std::vector<std::pair<int, char*> > data;
    xmlNodeSetPtr nodes = xpathObj->nodesetval;
    if (nodes && nodes->nodeNr > 0) {
        xmlNodePtr table = nodes->nodeTab[0];
        if (table) {
            for (xmlNodePtr child = table->children; child; child = child->next) {
                if (child->type == XML_ELEMENT_NODE)
                    processChild(child -> children, data);
            }
        }
    } else {
        printf("No <table> found for the XPath query: %s\n", xpathExpr);
    }

    // Cleanup
    xmlXPathFreeObject(xpathObj);
    xmlXPathFreeContext(xpathCtx);
    xmlFreeDoc(doc);
    xmlCleanupParser();

    // sort vector
    // write to csv

    return 0;
}