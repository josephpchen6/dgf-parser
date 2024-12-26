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
// fair assessment: player props will always have a relatively consistent amount of vig; they're also where most of the value is.
// we hard code these values, then use a line/curve/whatever of best fit for all else. This can also bake in moneyline/spread/etc

#define THRESHOLD -15

std::unordered_map<int, int> dkNba;
std::unordered_map<int, int> dkNfl;
std::unordered_map<int, int> fdNba;

void initMaps() {
    int key, value;
    FILE* dkNbaFp = fopen("dknba.bin", "rb");
    while (fread(&key, sizeof(int), 1, dkNbaFp) == 1 && fread(&value, sizeof(int), 1, dkNbaFp) == 1) {
        dkNba[key] = value;
        dkNba[value] = key;
    }
    fclose(dkNbaFp);
    FILE* dkNflFp = fopen("dknfl.bin", "rb");
    while (fread(&key, sizeof(int), 1, dkNflFp) == 1 && fread(&value, sizeof(int), 1, dkNflFp) == 1) {
        dkNfl[key] = value;
        dkNfl[value] = key;
    }
    fclose(dkNflFp);
    FILE* fdNbaFp = fopen("fdnba.bin", "rb");
    while (fread(&key, sizeof(int), 1, fdNbaFp) == 1 && fread(&value, sizeof(int), 1, fdNbaFp) == 1) {
        fdNba[key] = value;
        fdNba[value] = key;
    }
    fclose(fdNbaFp);
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
        if (!strncmp(league, "NBA", 4)) {
            if (fdNba.count(fdOdds)) {
                if (fdNba[fdOdds] > bestOdds) {
                    bestOdds = fdNba[fdOdds];
                }
            } else {
                printf("NOTE: Corresponding FD NBA odds not found: %d\n", fdOdds);
            }
        }
        // if (fdOdds > bestOdds)
        //     bestOdds = fdOdds;
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
                printf("NOTE: Corresponding DK NBA odds not found: %d\n", dkOdds);
            }
        } else if (!strncmp(league, "NFL", 4)) {
            if (dkNfl.count(dkOdds)) {
                if (dkNfl[dkOdds] > bestOdds) {
                    bestOdds = dkNfl[dkOdds];
                }
            } else {
                printf("NOTE: Corresponding DK NFL odds not found: %d\n", dkOdds);
            } 
        }
    }
    int arb = fliffOdds + bestOdds;
    if (arb >= THRESHOLD)
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
    
    initMaps();
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