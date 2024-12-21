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
// new idea:
// we check for specific vigs in specific leagues (NFL, NBA, etc.)
// if none of these are met, we go into a general formula

// do we even need a dict?

// other approach: see if vig is within a certain range
// i.e. if vig is between -110 to +110, then we just put 30 points of vig

#define THRESHOLD -20

std::unordered_map<int, int> dkNba;
std::unordered_map<int, int> dkNfl;

void initDkMap() {
    int key, value;
    FILE* fp = fopen("dknba.bin", "rb");
    while (fread(&key, sizeof(int), 1, fp) == 1 && fread(&value, sizeof(int), 1, fp) == 1) {
        dkNba[key] = value;
        dkNba[value] = key;
    }
    fclose(fp);
    fp = fopen("dknfl.bin", "rb");
    while (fread(&key, sizeof(int), 1, fp) == 1 && fread(&value, sizeof(int), 1, fp) == 1) {
        dkNfl[key] = value;
        dkNfl[value] = key;
    }
    fclose(fp);
}

void processChild(xmlNodePtr child, std::vector<std::pair<int, char*> >& data) {
    child = child -> next;
    int bestOdds = -10000;
    char* eventData = (char*)xmlNodeGetContent(child); // 1st index
    child = child -> next;
    char* league = (char*)xmlNodeGetContent(child); // 2nd index
    for (int i = 0; i < 4; i++)
        child = child -> next;
    int fliffOdds = atoi((char *)xmlNodeGetContent(child)); // 6th index
    for (int i = 0; i < 5; i++)
        child = child -> next;
    // pinnacle would be the 10th index
    if (child -> children -> children) {
        int fdOdds = atoi((char *)xmlNodeGetContent(child -> children -> children -> children -> children -> next)); // 11th index
        if (fdOdds > 0) {
            if (fdOdds <= 110) {
                fdOdds = (-1 * fdOdds) - 30;
            } else {
                // use formula
                fdOdds = INT_MIN;
            }
        } else {
            // say we have 110
            // the ideal outcome is 120
            // 
            if (fdOdds >= -140) {
                int diff = fdOdds + 130;
                if (diff < 0) {
                    fdOdds = 100 - diff;
                } else {
                    fdOdds = -100 - diff;
                }
            } else {
                // use formula
                fdOdds = INT_MIN;
            }
        }
        if (fdOdds > bestOdds)
            bestOdds = fdOdds;
    }
    child = child -> next;
    if (child -> children -> children) {
        int dkOdds = atoi((char *)xmlNodeGetContent(child -> children -> children -> children -> children -> next)); // 12th index
        if (!strncmp(league, "NBA", 4)) {
            if (dkNba.count(dkOdds)) {
                if (dkNba[dkOdds] > bestOdds) {
                    bestOdds = dkNba[dkOdds];
                }
            } else {
                printf("Corresponding NBA odds not found: %d\n", dkOdds);
            }
        } else if (!strncmp(league, "NFL", 4)) {
            if (dkNfl.count(dkOdds)) {
                if (dkNfl[dkOdds] > bestOdds) {
                    bestOdds = dkNfl[dkOdds];
                }
            } else {
                printf("Corresponding NFL odds not found: %d\n", dkOdds);
            } 
        }
    }
    int arb = fliffOdds + bestOdds;
    if (arb > THRESHOLD)
        data.push_back(std::make_pair(arb, eventData));
        // printf("market name: %s\nleague: %s\nfliff odds: %d\nbest odds: %d\n", eventData, league, fliffOdds, bestOdds);
        
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
    std::sort(data.rbegin(), data.rend());

    for (auto d : data) {
        printf("%d, %s\n", d.first, d.second);
    }
    // write to csv

    return 0;
}