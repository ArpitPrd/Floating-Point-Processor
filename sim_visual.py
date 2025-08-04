import csv
import matplotlib.pyplot as plt
from matplotlib.patches import Patch

def get_event_from_csv(filename:str) -> list[tuple]:
    instrs = []
    with open(filename, 'r') as file:
        reader = csv.reader(file)
        for row in reader:
            index = int(row[0])
            instr = row[1]
            issue = int(row[2])
            start = int(row[3])
            complete = int(row[4])
            writeback = int(row[5])

            instrs.append({
                "index":index,
                "instr":instr,
                "issue":issue,
                "start":start,
                "complete":complete,
                "writeback":writeback
            })

    return instrs

def plot_gantt(instrs:list[tuple], filename:str) -> None:

    _, ax = plt.subplots()

    colors = {
        "issue": "blue",
        "start": "orange",
        "complete": "green",
        "writeback": "red"
    }
    stage_height = 0.15
    gap = 0.05
      
    for i, inst in enumerate(instrs[1:]):
        y = i * (stage_height + gap)


        ax.broken_barh([(inst["issue"], 1)], (y, stage_height), facecolors=colors["issue"])


        duration = inst["complete"] - inst["start"] + 1
        if duration >= 0:
            ax.broken_barh([(inst["start"], duration)], (y, stage_height), facecolors=colors["start"])


        ax.broken_barh([(inst["complete"], 1)], (y, stage_height), facecolors=colors["complete"])


        ax.broken_barh([(inst["writeback"], 1)], (y, stage_height), facecolors=colors["writeback"])

        ax.text(-1, y + stage_height / 2, f'{inst["index"]}: {inst["instr"]}', va='center', ha='right')

    ax.set_xlabel("Clock Cycles")
    ax.set_yticks([])
    ax.set_title("Floating-Point Pipeline Timeline")
    ax.grid(True)
    
    legend_handles = [
        Patch(color=colors["issue"], label="issue"),
        Patch(color=colors["start"], label="start"),
        Patch(color=colors["complete"], label="complete"),
        Patch(color=colors["writeback"], label="writeback"),
    ]
    ax.legend(handles=legend_handles, loc="upper left")

    plt.tight_layout()

    print(f"saving the gantt chart at {filename}")
    
    plt.savefig(filename)

    return

if __name__ == "__main__":

    instr = get_event_from_csv("output.csv")
    plot_gantt(instr, "plot.png")