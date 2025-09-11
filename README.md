# If you need a python environment
```
# Init venv
python -m venv ~/myenv
# activate venv
source ~/myenv/bin/activate
# install requirements
pip install -r requirements.txt 
# Adding this venv to Jupyter
python -m ipykernel install --user --name=myenv
```

# If you want to use pre-defined plotting style
```
# I prefer using HOME
cd ~
mkdir lib
cd lib
git clone git@github.com:jedori0228/icarusplot.git
```
Then we should let Jupyter kernel know this path.
Edit `~/.local/share/jupyter/kernels/myenv/kernel.json` or equivalent to yours,
by adding "env" part like below:
```
{
 "argv": [
  "/home/jskim/myenv/bin/python",
  "-m",
  "ipykernel_launcher",
  "-f",
  "{connection_file}"
 ],
 "display_name": "myenv",
 "language": "python",
 "metadata": {
  "debugger": true
 },
 "env": {
  "PYTHONPATH": "/home/jskim/lib/LibraryPack/PythonLibraries/:/home/jskim/lib/icarusplot/"
 }
}
```
