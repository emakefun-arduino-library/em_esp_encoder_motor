import os
import subprocess
import shutil

workspace = os.path.join(os.path.dirname(__file__), os.pardir)

docs_path = os.path.join(workspace, "docs")

root_index: str = """
<!DOCTYPE html>
<html>

<head>
    <title>语言选择 (Language Selection)</title>
</head>

<body>
    <h1 style="font-size: larger; text-align: center;">请选择语言 (Please select a language):</h1>
    <p style="text-align: center;">
        <a href="./docs_zh-CN/index.html" style="font-size: larger;">中文 (Chinese)</a>
    </p>
    <p style="text-align: center;">
        <a href="./docs_en/index.html" style="font-size: larger;">英文 (English)</a>
    </p>
</body>

</html>
"""


if os.path.exists(docs_path):
    shutil.rmtree(docs_path)

os.mkdir(docs_path)

with open(
    os.path.join(docs_path, "index.html"),
    mode="w",
    encoding="utf-8",
    newline="\n",
) as file:
    file.write(root_index)

subprocess.run(
    ["doxygen", os.path.join("doxygen", "en.cfg")],
    shell=True,
    capture_output=False,
    text=True,
    check=True,
    cwd=workspace,
)

subprocess.run(
    ["doxygen", os.path.join("doxygen", "zh-CN.cfg")],
    shell=True,
    capture_output=False,
    text=True,
    check=True,
    cwd=workspace,
)
