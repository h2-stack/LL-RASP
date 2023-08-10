#CentOS Stream 8 x64

yum update -y
yum upgrade
yum install bison cmake gcc gdb -y

# Java Version Manager
curl "https://get.sdkman.io" | bash
sdk install java 6.0.119-zulu
sdk install java 8.0.302-open
sdk install java 7.0.342-zulu
sdk use java 8.0.302-open

# Node.js Version Manager
# git clone https://github.com/nodejs/node.git
curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.39.1/install.sh | bash
nvm install node
nvm install 14.7.0
nvm use 18

# Python Version Manager
wget https://www.python.org/ftp/python/2.0.1/Python-2.0.1.tgz
wget https://www.python.org/ftp/python/2.1.3/Python-2.1.3.tgz
wget https://www.python.org/ftp/python/2.2.3/Python-2.2.3.tgz
wget https://www.python.org/ftp/python/2.3.6/Python-2.3.6.tar.bz2
wget https://www.python.org/ftp/python/2.4.6/Python-2.4.6.tgz
wget https://www.python.org/ftp/python/2.5.6/Python-2.5.6.tar.bz2
wget https://www.python.org/ftp/python/2.6.9/Python-2.6.9.tgz
wget https://www.python.org/ftp/python/2.7.18/Python-2.7.18.tgz
wget https://www.python.org/ftp/python/3.0.1/Python-3.0.1.tgz
wget https://www.python.org/ftp/python/3.1.5/Python-3.1.5.tgz
wget https://www.python.org/ftp/python/3.2.6/Python-3.2.6.tgz
wget https://www.python.org/ftp/python/3.3.7/Python-3.3.7.tgz
wget https://www.python.org/ftp/python/3.4.10/Python-3.4.10.tgz
wget https://www.python.org/ftp/python/3.5.10/Python-3.5.10.tgz
wget https://www.python.org/ftp/python/3.6.15/Python-3.6.15.tgz
wget https://www.python.org/ftp/python/3.7.12/Python-3.7.12.tgz
wget https://www.python.org/ftp/python/3.8.13/Python-3.8.13.tgz
wget https://www.python.org/ftp/python/3.9.13/Python-3.9.13.tgz
wget https://www.python.org/ftp/python/3.10.5/Python-3.10.5.tgz

find . -name "Python*.bz2" -exec bzip2 -d {} \;
find . -name "Python*.tgz" -exec tar -zxf {} \;
find . -name "Python*.tar.gz" -exec tar -zxf {} \;
find . -name "Python*.tar" -exec tar -xf {} \;
# configure all version
find . -maxdepth 1 -name 'Python-*' -type d -exec bash -c 'pushd {};./configure;popd' \;
rm -rf *.tar *.tgz *.tar.gz

# PHP Version Manager
wget http://museum.php.net/php5/php-5.0.0.tar.gz
wget http://museum.php.net/php5/php-5.1.0.tar.gz
wget http://museum.php.net/php5/php-5.2.0.tar.bz2
wget http://museum.php.net/php5/php-5.3.0.tar.bz2
wget http://museum.php.net/php5/php-5.4.0.tar.bz2
wget http://museum.php.net/php5/php-5.5.0.tar.gz
wget https://www.php.net/distributions/php-5.6.0.tar.gz
wget https://www.php.net/distributions/php-7.0.0.tar.gz
wget https://www.php.net/distributions/php-7.1.0.tar.gz
wget https://www.php.net/distributions/php-7.2.0.tar.gz
wget https://www.php.net/distributions/php-7.3.0.tar.gz
wget https://www.php.net/distributions/php-7.4.0.tar.gz
wget https://www.php.net/distributions/php-8.0.0.tar.gz
wget https://www.php.net/distributions/php-8.1.0.tar.gz

find . -name "php*.bz2" -exec bzip2 -d {} \;
find . -name "php*.tgz" -exec tar -zxf {} \;
find . -name "php*.tar.gz" -exec tar -zxf {} \;
find . -name "php*.tar" -exec tar -xf {} \;
find . -maxdepth 1 -name 'php-*' -type d -exec bash -c 'pushd {};./configure;popd' \;
rm -rf *.tar *.tgz *.tar.gz

# Ruby Version Manager
wget https://cache.ruby-lang.org/pub/ruby/3.1/ruby-3.1.2.tar.gz
wget https://cache.ruby-lang.org/pub/ruby/3.0/ruby-3.0.4.tar.gz
wget https://cache.ruby-lang.org/pub/ruby/2.7/ruby-2.7.6.tar.gz

find . -name "ruby*.tar.gz" -exec tar -zxf {} \;
find . -maxdepth 1 -name 'ruby-*' -type d -exec bash -c 'pushd {};./configure;popd' \;
rm -rf *.tar.gz

