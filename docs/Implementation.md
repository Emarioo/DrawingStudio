The project is heavily based on optimization. That is because you very much do not want a slow or laggy program when designing.

Memory management will be very important. A base allocator for all memory may be important.

# Particles
Each pixel you draw is a particle with position and color. You can draw any where!

# Lines?
Lines instead of particles?

# Other thoughts

# Image chunks
Divide the draw space into chunks. Each chunk has an image. If there are no pixels in the chunk then an image does not need to be allocated.

The problem is if you only have one pixel in each chunk. A lot of wasted memory. The chunk needs to be chosen correctly.