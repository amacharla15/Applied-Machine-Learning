from datasets import load_dataset

OUT_PATH = "data/wikitext_10k.txt"
TARGET = 10000

ds = load_dataset("wikitext", "wikitext-103-raw-v1", split="train")

count = 0
with open(OUT_PATH, "w", encoding="utf-8") as f:
    for x in ds:
        text = x["text"].strip()
        if not text:
            continue
        if text.startswith("=") and text.endswith("="):
            continue
        text = " ".join(text.split())
        if not text:
            continue
        f.write(text + "\n")
        count += 1
        if count >= TARGET:
            break

print(f"Wrote {count} documents to {OUT_PATH}")
