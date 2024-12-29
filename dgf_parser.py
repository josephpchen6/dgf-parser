import pyperclip
from pathlib import Path
from bs4 import BeautifulSoup
import pandas as pd
from datetime import datetime

"""
- I only care about the lines of 2 books: FanDuel, DraftKings. All other books have too much vig.
    (Except for Pinnacle for non player props)
- I also don't really care about the average, just the best line

Sweaty Features:
- Hyper-optimize for FanDuel, DraftKings
    - Map with exact vig spreads
- Differentiate between markets
    - i.e. spread/totals will usually be sharper than player props = use different vig model

- Firefox copy inner body HTML extension? Kind of annoying to have to inspect element every time...
"""

def _positive_vig(plus_odds): # only call when under 115
    return (0.0023 * plus_odds * plus_odds) - (0.4843 * plus_odds) + 33.1885

def _negative_vig(minus_odds):
    return (-0.5437*minus_odds) - 82.9582

def calc_vig(odds):
    if (-200 <= odds <= 150):
        return 30
    elif (odds < 0):
        return _negative_vig(odds)
    else:
        return _positive_vig(odds)

def main():
    if Path("./source.txt").is_file():
        html_soup : BeautifulSoup = BeautifulSoup(open("source.txt", "r").read(), 'html.parser')
    else:
        html_soup : BeautifulSoup = BeautifulSoup(pyperclip.paste(), 'html.parser')

    events = html_soup.find_all(class_ = "transitions-colors group duration-75 hover:bg-brand-gray-2 bg-[#03161C]")
    lines = []

    for event in events:
        cols = event.contents
        fliff_odds = int(cols[6].text) # fliff line
        best_alternative = fliff_odds + 1
        
        pinnacle_odds = cols[10].find(class_ = "mt-1.5 w-full text-center rounded-full border border-[#4F5253] bg-[#112B35] px-2 py-0.5 text-base font-semibold text-white")
        if pinnacle_odds:
            pinnacle_odds = int(pinnacle_odds.text)
            if (pinnacle_odds > fliff_odds):
                continue
            elif (pinnacle_odds < best_alternative):
                best_alternative = pinnacle_odds
        
        fanduel_odds = cols[11].find(class_ = "mt-1.5 w-full text-center rounded-full border border-[#4F5253] bg-[#112B35] px-2 py-0.5 text-base font-semibold text-white")
        if fanduel_odds:
            fanduel_odds = int(fanduel_odds.text)
            if (fanduel_odds > fliff_odds):
                continue
            elif (fanduel_odds < best_alternative):
                best_alternative = fanduel_odds
        
        draftkings_odds = cols[11].find(class_ = "mt-1.5 w-full text-center rounded-full border border-[#4F5253] bg-[#112B35] px-2 py-0.5 text-base font-semibold text-white")
        if draftkings_odds:
            draftkings_odds = int(draftkings_odds.text)
            if (draftkings_odds > fliff_odds):
                continue
            elif (draftkings_odds < best_alternative):
                best_alternative = draftkings_odds

        if (best_alternative < fliff_odds): # if not True: i can get better odds elsewhere
            estimated_vig = calc_vig(best_alternative)
            best_alternative = -1 * (best_alternative + estimated_vig)
            # if (0 < best_alternative < 100):
            #     best_alternative = 200 - best_alternative
            lines.append({"Name" : cols[1].get_text("; "), "Fliff Odds" : fliff_odds, "Best Alternative" : best_alternative})
    
    output = pd.DataFrame(lines)
    output["Diff"] = output["Fliff Odds"] + output["Best Alternative"]
    output.sort_values("Diff", ascending = False, inplace = True)
    output.to_csv(f"/Users/josephchen/Desktop/projections/{datetime.now().strftime('%Y-%m-%d;%H-%M')}_DGF.csv")
    print("Done!")

main()
