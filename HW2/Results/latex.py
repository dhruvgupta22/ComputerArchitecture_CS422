import re
import sys

def parse_data1(data):
    direction_predictors = []
    target_predictors = []
    time = 0
    lines = data.strip().split("\n")
    section = None
    for line in lines:
        if "Time taken" in line:
            match = re.match(r"Time taken : ([^)]+) seconds.", line)
            if match:
                time = match.group(1)
        elif "Direction Predictors" in line:
            section = "direction"
        elif "Branch Target Predictors" in line:
            section = "target"
        elif section == "direction":
            match = re.match(r"(\w+) : Accesses (\d+), Mispredictions (\d+) \(([^)]+)\), Forward Branches (\d+), Forward Mispredictions (\d+) \(([^)]+)\), Backward Branches (\d+), Backward Mispredictions (\d+) \(([^)]+)\)", line)
            if match:
                name = match.group(1)
                access = match.group(2)
                mispred = match.group(3)
                mispred_percent = match.group(4)
                forw_br = match.group(5)
                forw_mispred = match.group(6)
                forw_mispred_percent = match.group(7)
                back_br = match.group(8)
                back_mispred = match.group(9)
                back_mispred_percent = match.group(10)
                direction_predictors.append((name, access, mispred, mispred_percent, forw_br, forw_mispred, forw_mispred_percent, back_br, back_mispred, back_mispred_percent))

        elif section == "target":
            match = re.match(r"(\w+) : Accesses (\d+), Misses (\d+) \(([^)]+)\), Mispredictions (\d+) \(([^)]+)\)", line)
            if match:
                name = match.group(1)
                access = match.group(2)
                misses = match.group(3)
                miss_perc = match.group(4)
                mispredictions = match.group(5)
                misprediction_perc = match.group(6)
                target_predictors.append((name, access, misses, miss_perc, mispredictions, misprediction_perc))
    return direction_predictors, target_predictors, time

def generate_latex(direction_predictors, target_predictors, time):
    latex = f"Forward branches = {direction_predictors[0][4]} \\\\\nBackward branches = {direction_predictors[0][7]} \\\\\nTotal accesses = {direction_predictors[0][1]}\n"
    latex += """
\\begin{table}[h]
    \\centering
    \\renewcommand{\\arraystretch}{1.3}
    \\begin{tabular}{|c|c c|c c|c c|}
        \\hline
        \\multirow{2}{*}{Direction Predictors} & \\multicolumn{2}{c|}{Forward Mispredictions} & \\multicolumn{2}{c|}{Backward Mispredictions} & \\multicolumn{2}{c|}{Total Mispredictions} \\\\
        \\cline{2-7}
        & Count & Fraction & Count & Fraction & Count & Fraction \\\\
        \\hline
"""
    for dp in direction_predictors:
        latex += f"            \\textbf{{{dp[0]}}} & {dp[5]} & {dp[6]} & {dp[8]} & {dp[9]} & {dp[2]} & {dp[3]} \\\\\n"
    latex += "        \\hline\n        \\end{tabular}\n    \\end{table}\n"
    
    latex += """
\\subsection*{Part B: Target Predictors for indirect control flow instructions}
"""
    latex += f"Accesses = {target_predictors[0][1]} \\\\"
    latex += """
\\begin{table}[h]
    \\centering
    \\renewcommand{\\arraystretch}{1.3}
    \\begin{tabular}{|c|c c|c c|}
        \\hline
        \\multirow{2}{*}{Target Predictors} & \\multicolumn{2}{c|}{Misses} & \\multicolumn{2}{c|}{Mispredictions} \\\\
        \\cline{2-5}
        & Count & Fraction & Count & Fraction \\\\
        \\hline
"""
    for tp in target_predictors:
        latex += f"            \\textbf{{{tp[0]}}}  & {tp[2]} & {tp[3]} & {tp[4]} & {tp[5]} \\\\\n"
    latex += "        \\hline\n        \\end{tabular}\n    \\end{table}\n"
    latex += """
\\begin{itemize}
\\end{itemize}
"""
    latex += f"Total Time taken (seconds) = {time} \\\\"

    return latex

def main():
    benchmarks = ["400.perlbench", "401.bzip", "403.gcc", "429.mcf", "450.soplex", "456.hmmer", "471.omnetpp", "483.xalancbmk"]
    input_files = [b + ".txt" for b in benchmarks]
    output_files = [b + "_latex.txt" for b in benchmarks]
    
    for input_file, output_file in zip(input_files, output_files):
        with open(input_file, 'r') as f:
            data1 = f.read()
        
        direction_predictors, target_predictors, time = parse_data1(data1)
        latex_output = generate_latex(direction_predictors, target_predictors, time)
        
        with open(output_file, 'w') as f:
            f.write(latex_output)
    
    print("All files processed successfully.")

if __name__ == "__main__":
    main()
