# DGF Parser

Script to identify potential arbs using the Daily Grind Fantasy Optimizer, written in C++. Mainly aimed at NBA player props, but some NFL and NCAAB are supported. Works for anything that has a "league" column in DGF (Fliff, Novig, etc.)

Setup:

Get the XPath of the \<tbody> element.

<details>
<summary><b>Visual Tutorial (Click me!!)</b></summary>
Inspect element; click on tags until the table lights up. It should be somewhere around here on the source:
   
<img width="1440" alt="Screenshot 2025-01-29 at 6 34 18 PM" src="https://github.com/user-attachments/assets/e1a4ad5e-81b8-4354-84aa-b57dac962e9b" />


Then, right-click and select Copy > XPath


<img width="1440" alt="Screenshot 2025-01-29 at 6 34 22 PM" src="https://github.com/user-attachments/assets/10b259b7-1854-4d1d-9d43-c320fd9376ac" />

</details>

Copy this to line 200 of the script, inside the quotes.

## To run the parser:

- Copy the body's innerHTML on DGF. You can keep using Inspecting Element (slow), or this Firefox extension I made (fast):
   
   https://addons.mozilla.org/en-US/firefox/addon/copy-body-innerhtml/

   (Source is [here](https://github.com/josephpchen6/copy_body_innerhtml))

- Run the script; it will automatically parse the clipboard contents. (If you want to develop, then paste into sample.html; the script will read from there.)

## To-Do:

Better line exists/EV on all sites flag

Add more Pinnacle leagues

Tennis, Soccer lines

Player blacklists?