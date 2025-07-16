# How to deploy

* Build the Linux Server and put the content under ./server/server

* Current IP: 18.220.228.236
  
* Zip this ```Deploy``` folder to a zip file

* Copy the zip file to the ```EC2``` machine.
```sh
scp -i path/to/your_ssh_key.pem /path/to/the/Deploy.zip ec2-user@your-ec2-public-ip:/home/ec2-user
```

* Virtual Machine commands.
```sh
ls | show content
cd .. | move out of folder
rm (file name) | delete file
rm -rf (folder name) | delete folder
clear | clears command line
unzip (zipped folder) | Unzips folder
ctrl + c | cancel current process
pwd | show current directory
```

* Check what's taking up space, then remove everything: 
```sh
docker ps -a 
docker logs <name>
docker system df
docker system prune --all --force
docker system prune --all --force --volumes
```

* log in to the ```EC2``` machine:
```sh
ssh -i path/to/your_ssh_key.pem ec2-user@your-ec2-public-ip
```

* navigate to the home:
```sh
cd
```

* unzip the file:
```sh
unzip Deploy.zip
```

* get into the unzipped folder:
```sh
cd Deploy
```

* make the server excutable and excutable:
```sh
chmod +x /server/server/Rabies_ANGD/Binaries/Linux/Rabies_ANGDServer
```

* build the services:
```sh
docker-compose build
```

* launch the coordinator:
```sh
docker-compose up coordinator
```
