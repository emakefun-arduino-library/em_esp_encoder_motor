import os
import subprocess
import shutil

workspace = os.path.join(os.path.dirname(__file__), os.pardir)


def rmdir(path):
    if os.path.exists(path):
        shutil.rmtree(path)


rmdir(os.path.join(workspace, "docs_en"))

rmdir(os.path.join(workspace, "docs_zh-CN"))

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
