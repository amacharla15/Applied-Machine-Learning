from transformers import GPT2TokenizerFast

tok = GPT2TokenizerFast.from_pretrained("gpt2")

tests = [
    "what are you doing?",
    "don't do that !",
    "Hello, world!",
    "chatGpt is cool.",
]

for text in tests:
    ids = tok.encode(text, add_special_tokens=False)
    print("TEXT:", repr(text))
    print("IDS :", ids)
    print()