#include <stdio.h>
#include <libxml/HTMLparser.h>
#include <libxml/xpath.h>
#include <cstdlib>
#include <utility>
#include <vector>
#include <unordered_map>

// TO-DO:
// add pinnacle
// tennis, soccer (other?)
// filter sports by in season (would it really save that much time/computing power)

#define THRESHOLD -15
#define DEFAULT_ODDS -10000
// all lines below this threshold (i.e. more negative than) will be ignored
// decrease the threshold to see more potential arbs, increase to see less

// NOTE: a 200 arb = 0 (+100/+100)

std::unordered_map<int, int> fdNba;
std::unordered_map<int, int> fdNfl;
std::unordered_map<int, int> fdNcaaf;
std::unordered_map<int, int> fdNcaab;
std::unordered_map<int, int> fdNhl;

std::unordered_map<int, int> dkNba;
std::unordered_map<int, int> dkNfl;
std::unordered_map<int, int> dkNcaaf;
std::unordered_map<int, int> dkNcaab;
std::unordered_map<int, int> dkNhl;

std::unordered_map<int, int> pinNba;

void initMap(std::unordered_map<int, int>& to_fill, const char* filename) {
    int key, value;
    FILE* fp = fopen(filename, "rb");
    while (fread(&key, sizeof(int), 1, fp) == 1 && fread(&value, sizeof(int), 1, fp) == 1) {
        to_fill[key] = value;
        to_fill[value] = key;
    }
    fclose(fp);
}

void initMaps() {
    initMap(fdNba, "fdnba.bin");
    initMap(fdNfl, "fdnfl.bin");
    initMap(fdNcaaf, "fdncaaf.bin");
    initMap(fdNcaab, "fdncaab.bin");
    initMap(fdNhl, "fdnhl.bin");

    initMap(dkNba, "dknba.bin");
    initMap(dkNfl, "dknfl.bin");
    initMap(dkNcaaf, "dkncaaf.bin");
    initMap(dkNcaab, "dkncaab.bin");
    initMap(dkNhl, "dknhl.bin");

    initMap(pinNba, "pinnba.bin");
}

void processChild(xmlNodePtr child, std::vector<std::pair<int, char*> >& data) {
    child = child -> next;
    int bestOdds = DEFAULT_ODDS;
    int previousBest = DEFAULT_ODDS;
    char* eventData = (char*)xmlNodeGetContent(child); // 1st index
    child = child -> next;
    char* league = (char*)xmlNodeGetContent(child); // 2nd index
    char site = 0;
    for (int i = 0; i < 4; i++)
        child = child -> next;
    int fliffOdds = atoi((char *)xmlNodeGetContent(child)); // 6th index
    for (int i = 0; i < 4; i++)
        child = child -> next;

    if (child -> children -> children) { // process Pinnacle
        int pinOdds = atoi((char *)xmlNodeGetContent(child -> children -> children -> children -> children -> next)); // 10th index
        if (!strncmp(league, "NBA", 4)) {
            if (pinNba.count(pinOdds)) {
                if (pinNba[pinOdds] > bestOdds)
                    bestOdds = pinNba[pinOdds];
            } else {
                printf("[WARN] Corresponding Pin NBA odds not found: %d\n", pinOdds);
            }
        } else {
            // printf("Pin %s not found: %s\n", league, eventData);
        }
    }
    child = child -> next;
    if (bestOdds != previousBest) {
        site = 1;
        previousBest = bestOdds;
    }

    if (child -> children -> children) { // process FanDuel
        int fdOdds = atoi((char *)xmlNodeGetContent(child -> children -> children -> children -> children -> next)); // 11th index
        if (!strncmp(league, "NBA", 4)) {
            if (fdNba.count(fdOdds)) {
                if (fdNba[fdOdds] > bestOdds)
                    bestOdds = fdNba[fdOdds];
            } else {
                printf("[WARN] Corresponding FD NBA odds not found: %d\n", fdOdds);
            }
        } else if (!strncmp(league, "NFL", 4)) {
                // printf("%d\n", fdOdds);
            if (fdNfl.count(fdOdds)) {
                if (fdNfl[fdOdds] > bestOdds)
                    bestOdds = fdNfl[fdOdds];
            } else {
                printf("[WARN] Corresponding FD NFL odds not found: %d\n", fdOdds);
            }
        } else if (!strncmp(league, "NCAAF", 6)) {
                // printf("%d\n", fdOdds);
            if (fdNcaaf.count(fdOdds)) {
                if (fdNcaaf[fdOdds] > bestOdds)
                    bestOdds = fdNcaaf[fdOdds];
            } else {
                printf("[WARN] Corresponding FD NCAAF odds not found: %d\n", fdOdds);
            }
        } else if (!strncmp(league, "NCAAB", 6)) {
            if (fdNcaab.count(fdOdds)) {
                if (fdNcaab[fdOdds] > bestOdds)
                    bestOdds = fdNcaab[fdOdds];
            } else {
                printf("[WARN] Corresponding FD NCAAB odds not found: %d\n", fdOdds);
            }
        } else if (!strncmp(league, "NHL", 4)) {
            if (fdNhl.count(fdOdds)) {
                if (fdNhl[fdOdds] > bestOdds)
                    bestOdds = fdNhl[fdOdds];
            } else {
                printf("[WARN] Corresponding FD NHL odds not found: %d\n", fdOdds);
            }
        } else {
            // printf("FD %s not found: %s\n", league, eventData);
        }
        if (bestOdds != previousBest) {
            site = 2;
            previousBest = bestOdds;
        }
    }
    child = child -> next;

    if (child -> children -> children) { // process DraftKings
        int dkOdds = atoi((char *)xmlNodeGetContent(child -> children -> children -> children -> children -> next)); // 12th index
        if (!strncmp(league, "NBA", 4)) {
            if (dkNba.count(dkOdds)) {
                if (dkNba[dkOdds] > bestOdds) {
                    bestOdds = dkNba[dkOdds];
                }
            } else {
                printf("[WARN] Corresponding DK NBA odds not found: %d\n", dkOdds);
            }
        } else if (!strncmp(league, "NFL", 4)) {
            if (dkNfl.count(dkOdds)) {
                if (dkNfl[dkOdds] > bestOdds)
                    bestOdds = dkNfl[dkOdds];
            } else {
                printf("[WARN] Corresponding DK NFL odds not found: %d\n", dkOdds);
            } 
        } else if (!strncmp(league, "NCAAF", 6)) {
            if (dkNcaaf.count(dkOdds)) {
                if (dkNcaaf[dkOdds] > bestOdds)
                    bestOdds = dkNcaaf[dkOdds];
            } else {
                printf("[WARN] Corresponding DK NCAAF odds not found: %d\n", dkOdds);
            } 
        } else if (!strncmp(league, "NCAAB", 6)) {
            if (dkNcaab.count(dkOdds)) {
                if (dkNcaab[dkOdds] > bestOdds)
                    bestOdds = dkNcaab[dkOdds];
            } else {
                printf("[WARN] Corresponding DK NCAAB odds not found: %d\n", dkOdds);
            } 
        } else if (!strncmp(league, "NHL", 4)) {
            if (dkNhl.count(dkOdds)) {
                if (dkNhl[dkOdds] > bestOdds)
                    bestOdds = dkNhl[dkOdds];
            } else {
                printf("[WARN] Corresponding DK NHL odds not found: %d\n", dkOdds);
            } 
        } else {
            // printf("DK %s not found: %s\n", league, eventData);
        }
        if (bestOdds != previousBest) {
            site = 4;
            // no need to update previousBest
        }
    }

    int arb = fliffOdds + bestOdds;
    if (arb >= THRESHOLD)
        data.push_back(std::make_pair((arb % 200 << 3) | site, eventData));
        // printf("market name: %s\nleague: %s\nfliff odds: %d\nbest odds: %d\n", eventData, league, fliffOdds, bestOdds);
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

    const char *xpathExpr = "/html/body/div[4]/div/div/main/div/div[2]/div/div/div/table/tbody";
    // EDIT THIS LINE: SEE README
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

    printf("\nOutput:\n");
    // sort vector
    std::sort(data.begin(), data.end());
    for (const std::pair<int, char*>& d : data) {
        if (d.first & 1) {
            printf("(Pin) ");
        } else if (d.first & 2) {
            printf("(FD) ");
        } else if (d.first & 4) {
            printf("(DK) ");
        }
        printf("%d  \t%s\n", d.first >> 3, d.second);
    }
    return 0;
}